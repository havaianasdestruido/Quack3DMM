#include "event/event_bus.h"

#include <cassert>
#include <vector>

int main() {
    using namespace q3d::event;
    auto &bus = EventBus::instance();
    bus.unsubscribe_all();
    bus.enable(SceneLoaded::StaticID());

    std::vector<int> order;
    auto s1 = bus.on<SceneLoaded>("m1", [&](const SceneLoaded &) { order.push_back(1); }, 1);
    auto s2 = bus.on<SceneLoaded>("m2", [&](const SceneLoaded &) { order.push_back(2); }, 3);
    bus.fire(SceneLoaded{std::string("scene")});
    assert((order == std::vector<int>{2, 1}));

    bus.unsubscribe(s1);
    bus.unsubscribe(s2);
    bool ran = false;
    bus.on<SceneSaved>("m", [&](const SceneSaved &) { ran = true; }, 0);
    bus.disable(SceneSaved::StaticID());
    bus.fire(SceneSaved{std::string("x")});
    assert(!ran);
    bus.enable(SceneSaved::StaticID());

    int tick_count = 0;
    bus.set_throttle(TickPre::StaticID(), 1);
    bus.on<TickPre>("m", [&](const TickPre &) { ++tick_count; }, 0);
    bus.fire(TickPre{0.1f});
    bus.fire(TickPre{0.1f});
    assert(tick_count <= 1);

    int replay_count = 0;
    bus.on<AssetLoaded>("m", [&](const AssetLoaded &) { ++replay_count; }, 0);
    bus.start_recording();
    bus.fire(AssetLoaded{std::string("a")});
    bus.stop_recording();
    bus.replay();
    assert(replay_count >= 2);

    int seen = 0;
    bus.on<ActorCreated>("m", [&](const ActorCreated &) { ++seen; }, 0);
    bus.fire(ActorCreated{std::string("id")});
    bus.fire(ActorDestroyed{std::string("id")});
    assert(seen == 1);

    return 0;
}