#include "event_bus.h"

#include <algorithm>
#include <chrono>

namespace q3d {
namespace event {

EventBus &EventBus::instance() {
    static EventBus bus;
    return bus;
}

void EventBus::subscribe_internal(EventID id, std::unique_ptr<Handler> h, const std::string &mod_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto *raw = h.get();
    int priority = 0;
    auto &vec = handlers_[id];
    vec.emplace_back(priority, std::move(h));
    handler_owners_[raw] = { id, mod_id };
    std::sort(vec.begin(), vec.end(), [](const HandlerEntry &a, const HandlerEntry &b) {
        return a.first > b.first;
    });
}

void EventBus::fire_internal(EventID id, AnyEvent *event) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (disabled_.count(id)) return;

    auto now = std::chrono::steady_clock::now().time_since_epoch();
    long ms = static_cast<long>(std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
    auto itThrottle = throttle_.find(id);
    if (itThrottle != throttle_.end()) {
        auto last = last_fire_[id];
        long diff = ms - last;
        if (diff < 1000 / std::max(1, itThrottle->second)) return;
        last_fire_[id] = ms;
    } else {
        last_fire_[id] = ms;
    }

    auto it = handlers_.find(id);
    if (it == handlers_.end()) return;
    for (auto &entry : it->second) {
        entry.second->handle(event);
    }

    if (recording_) {
        recorded_.emplace_back(id, event->clone());
    }
}

Cancelled EventBus::fire_cancellable_internal(EventID id, AnyEvent *event) {
    fire_internal(id, event);
    return false;
}

EventBus::Subscription EventBus::on_pattern(const char *mod_id, const char *pattern,
                                            std::function<void(EventID, const AnyEvent *)> cb,
                                            int priority) {
    struct PatternHandler : Handler {
        std::string pattern;
        std::function<void(EventID, const AnyEvent *)> cb;
        int priority;
        PatternHandler(std::string p, std::function<void(EventID, const AnyEvent *)> c, int pr)
            : pattern(std::move(p)), cb(std::move(c)), priority(pr) {}
        void handle(const AnyEvent *event) override { (void)event; }
        bool can_handle(EventID id) const override {
            return id.find(pattern) != std::string::npos;
        }
    };

    auto h = std::make_unique<PatternHandler>(pattern, std::move(cb), priority);
    auto *raw = h.get();
    subscribe_internal(pattern, std::move(h), mod_id ? mod_id : "");
    return { pattern, priority, raw };
}

void EventBus::fire_raw(EventID id, const AnyEvent *event) {
    fire_internal(id, const_cast<AnyEvent *>(event));
}

bool EventBus::cancel(EventID id) {
    std::lock_guard<std::mutex> lock(mutex_);
    disabled_.insert(id);
    return true;
}

void EventBus::unsubscribe(const Subscription &sub) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto itOwner = handler_owners_.find(sub.callback_ptr);
    if (itOwner == handler_owners_.end()) return;
    auto it = handlers_.find(itOwner->second.first);
    if (it == handlers_.end()) {
        handler_owners_.erase(itOwner);
        return;
    }
    auto &vec = it->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const HandlerEntry &e) {
        return e.second.get() == sub.callback_ptr;
    }), vec.end());
    handler_owners_.erase(itOwner);
}

void EventBus::unsubscribe_mod(const char *mod_id) {
    if (!mod_id) return;
    std::lock_guard<std::mutex> lock(mutex_);
    std::string mod(mod_id);
    for (auto it = handler_owners_.begin(); it != handler_owners_.end();) {
        if (it->second.second == mod) {
            auto hit = handlers_.find(it->second.first);
            if (hit != handlers_.end()) {
                auto &vec = hit->second;
                vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const HandlerEntry &e) {
                    return e.second.get() == it->first;
                }), vec.end());
            }
            it = handler_owners_.erase(it);
        } else {
            ++it;
        }
    }
}

void EventBus::unsubscribe_all() {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.clear();
    handler_owners_.clear();
}

std::vector<std::pair<EventID, std::string>> EventBus::list_subscriptions() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::pair<EventID, std::string>> out;
    for (const auto &entry : handler_owners_) {
        out.emplace_back(entry.second.first, entry.second.second);
    }
    return out;
}

EventBus::EventInfo EventBus::describe(EventID id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    EventInfo info{};
    info.id = id;
    auto it = handlers_.find(id);
    info.handler_count = (it != handlers_.end()) ? static_cast<int>(it->second.size()) : 0;
    info.cancellable = true;
    info.description = "";
    return info;
}

void EventBus::enable(EventID id) {
    std::lock_guard<std::mutex> lock(mutex_);
    disabled_.erase(id);
}

void EventBus::disable(EventID id) {
    std::lock_guard<std::mutex> lock(mutex_);
    disabled_.insert(id);
}

void EventBus::start_recording() {
    std::lock_guard<std::mutex> lock(mutex_);
    recording_ = true;
    recorded_.clear();
}

void EventBus::stop_recording() {
    std::lock_guard<std::mutex> lock(mutex_);
    recording_ = false;
}

void EventBus::replay() {
    std::vector<std::pair<EventID, std::unique_ptr<AnyEvent>>> snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        snapshot.swap(recorded_);
    }
    // Re-fire each recorded event after releasing the mutex so
    // fire_internal can re-acquire it without deadlocking.
    for (auto &e : snapshot) {
        AnyEvent *raw = e.second.get();
        if (raw)
            fire_internal(e.first, raw);
    }
}

void EventBus::set_throttle(EventID id, int max_per_second) {
    std::lock_guard<std::mutex> lock(mutex_);
    throttle_[id] = max_per_second;
}

} // namespace event
} // namespace q3d
