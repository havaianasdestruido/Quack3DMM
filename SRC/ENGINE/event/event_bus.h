#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <mutex>

namespace q3d {
namespace event {

struct AnyEvent { virtual ~AnyEvent() = default; virtual std::unique_ptr<AnyEvent> clone() const = 0; };

template<typename T>
struct Event : AnyEvent {
    T value;
    explicit Event(const T &v) : value(v) {}

    std::unique_ptr<AnyEvent> clone() const override {
        return std::make_unique<Event<T>>(value);
    }
};

using EventID = std::string;
using Cancelled = bool;
using HookCallback = std::function<void()>;

class Handler {
public:
    virtual ~Handler() = default;
    virtual void handle(const AnyEvent *event) = 0;
    virtual bool can_handle(EventID id) const = 0;
};

template<typename T>
class TypedHandler : public Handler {
public:
    using Callback = std::function<void(const T &)>;

    TypedHandler(Callback cb, int priority = 0) : cb_(std::move(cb)), priority_(priority) {}

    void handle(const AnyEvent *event) override {
        auto *e = dynamic_cast<const Event<T> *>(event);
        if (e) cb_(e->value);
    }

    bool can_handle(EventID id) const override { return id == T::StaticID(); }

    Callback cb_;
    int priority_;
};

class EventBus {
public:
    struct Subscription {
        EventID id;
        int priority;
        void *callback_ptr;
    };

    static EventBus &instance();

    template<typename T>
    Subscription on(const char *mod_id, typename TypedHandler<T>::Callback cb, int priority = 0) {
        auto h = std::make_unique<TypedHandler<T>>(std::move(cb), priority);
        auto *raw = h.get();
        subscribe_internal(T::StaticID(), std::move(h), mod_id ? mod_id : "");
        return { T::StaticID(), priority, raw };
    }

    Subscription on_pattern(const char *mod_id, const char *pattern,
                            std::function<void(EventID, const AnyEvent *)> cb,
                            int priority = 0);

    template<typename T>
    void fire(const T &payload) {
        auto e = std::make_unique<Event<T>>(payload);
        fire_internal(T::StaticID(), e.get());
    }

    template<typename T>
    Cancelled fire_cancellable(const T &payload) {
        auto e = std::make_unique<Event<T>>(payload);
        return fire_cancellable_internal(T::StaticID(), e.get());
    }

    void fire_raw(EventID id, const AnyEvent *event);

    bool cancel(EventID id);

    void unsubscribe(const Subscription &sub);
    void unsubscribe_mod(const char *mod_id);
    void unsubscribe_all();

    std::vector<std::pair<EventID, std::string>> list_subscriptions() const;

    struct EventInfo {
        EventID id;
        int handler_count;
        bool cancellable;
        std::string description;
    };

    EventInfo describe(EventID id) const;

    void enable(EventID id);
    void disable(EventID id);

    void start_recording();
    void stop_recording();
    void replay();

    void set_throttle(EventID id, int max_per_second);

private:
    void subscribe_internal(EventID id, std::unique_ptr<Handler> h, const std::string &mod_id);
    Cancelled fire_cancellable_internal(EventID id, AnyEvent *event);

    void fire_internal(EventID id, AnyEvent *event);

    using HandlerEntry = std::pair<int, std::unique_ptr<Handler>>;

    std::unordered_map<EventID, std::vector<HandlerEntry>> handlers_;
    std::unordered_map<void *, std::pair<EventID, std::string>> handler_owners_;
    std::unordered_set<EventID> disabled_;
    std::unordered_map<EventID, int> throttle_;
    std::unordered_map<EventID, long> last_fire_;

    bool recording_ = false;
    std::vector<std::pair<EventID, std::unique_ptr<AnyEvent>>> recorded_;

    mutable std::mutex mutex_;
};

#define Q3D_EVENT(NAME, ...) \
    struct NAME { \
        using ThisType = NAME; \
        static EventID StaticID() { return #NAME; } \
        __VA_ARGS__ value; \
    };

Q3D_EVENT(SceneLoaded, std::string)
Q3D_EVENT(SceneSaved, std::string)
Q3D_EVENT(SceneFrameAdvanced, int)
Q3D_EVENT(ScenePlayStarted, int)
Q3D_EVENT(ScenePlayStopped, int)
Q3D_EVENT(ScenePlayPaused, int)

Q3D_EVENT(ActorCreated, std::string)
Q3D_EVENT(ActorDestroyed, std::string)
Q3D_EVENT(ActorMoved, std::string)
Q3D_EVENT(ActorRotated, std::string)
Q3D_EVENT(ActorScaled, std::string)
Q3D_EVENT(ActorAnimationChanged, std::string)
Q3D_EVENT(ActorSelected, std::string)
Q3D_EVENT(ActorDeselected, std::string)
Q3D_EVENT(ActorCostumeChanged, std::string)
Q3D_EVENT(ActorMaterialChanged, std::string)

Q3D_EVENT(ToolActivated, std::string)
Q3D_EVENT(ToolDeactivated, std::string)
Q3D_EVENT(SelectionChanged, std::vector<std::string>)
Q3D_EVENT(PropertyEdited, std::string)
Q3D_EVENT(UndoPerformed, int)
Q3D_EVENT(RedoPerformed, int)
Q3D_EVENT(AssetDropped, std::string)
Q3D_EVENT(UndoRedoPerformed, std::string)

Q3D_EVENT(TickPre, float)
Q3D_EVENT(TickPost, float)
Q3D_EVENT(RenderPre, int)
Q3D_EVENT(RenderPost, int)
Q3D_EVENT(AudioMixed, int)

Q3D_EVENT(AssetLoaded, std::string)
Q3D_EVENT(AssetUnloaded, std::string)
Q3D_EVENT(AssetMissing, std::string)

Q3D_EVENT(ModLoaded, std::string)
Q3D_EVENT(ModUnloaded, std::string)
Q3D_EVENT(ModError, std::pair<std::string, std::string>)
Q3D_EVENT(ModReloaded, std::string)

Q3D_EVENT(UIButtonClicked, std::string)
Q3D_EVENT(UIPanelOpened, std::string)
Q3D_EVENT(UIPanelClosed, std::string)

#undef Q3D_EVENT

} // namespace event
} // namespace q3d
