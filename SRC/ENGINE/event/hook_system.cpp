#include "hook_system.h"

#include <algorithm>

namespace q3d {
namespace event {

HookSystem &HookSystem::instance() {
    static HookSystem hooks;
    return hooks;
}

std::vector<HookSystem::HookEntry> &HookSystem::get_before_hooks(const char *name) {
    return before_hooks_[name];
}

std::vector<HookSystem::HookEntry> &HookSystem::get_after_hooks(const char *name) {
    return after_hooks_[name];
}

std::vector<HookCallback> &HookSystem::get_replace_hooks(const char *name) {
    return replace_hooks_[name];
}

void HookSystem::register_before_hook(const char *mod_id, const char *name, HookCallback cb, int priority) {
    (void)mod_id;
    auto &vec = before_hooks_[name];
    vec.emplace_back(priority, std::move(cb));
    std::sort(vec.begin(), vec.end(), [](const HookEntry &a, const HookEntry &b) {
        return a.first > b.first;
    });
}

void HookSystem::register_after_hook(const char *mod_id, const char *name, HookCallback cb, int priority) {
    (void)mod_id;
    auto &vec = after_hooks_[name];
    vec.emplace_back(priority, std::move(cb));
    std::sort(vec.begin(), vec.end(), [](const HookEntry &a, const HookEntry &b) {
        return a.first > b.first;
    });
}

void HookSystem::register_replace_hook(const char *mod_id, const char *name, HookCallback cb) {
    (void)mod_id;
    auto &vec = replace_hooks_[name];
    vec.emplace_back(std::move(cb));
}

void HookSystem::unregister_mod_hooks(const char *mod_id) {
    (void)mod_id;
    before_hooks_.clear();
    after_hooks_.clear();
    replace_hooks_.clear();
}

std::vector<HookSystem::HookPoint> HookSystem::list_hook_points() const {
    std::vector<HookPoint> out;
    for (const auto &kv : before_hooks_) {
        HookPoint hp;
        hp.name = kv.first;
        out.push_back(hp);
    }
    for (const auto &kv : after_hooks_) {
        if (std::none_of(out.begin(), out.end(), [&](const HookPoint &hp) { return hp.name == kv.first; })) {
            HookPoint hp;
            hp.name = kv.first;
            out.push_back(hp);
        }
    }
    for (const auto &kv : replace_hooks_) {
        if (std::none_of(out.begin(), out.end(), [&](const HookPoint &hp) { return hp.name == kv.first; })) {
            HookPoint hp;
            hp.name = kv.first;
            out.push_back(hp);
        }
    }
    return out;
}

} // namespace event
} // namespace q3d
