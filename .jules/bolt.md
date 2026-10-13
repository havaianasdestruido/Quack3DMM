## 2024-05-24 - EventBus unnecessary overhead
**Learning:** The `EventBus::fire_internal` method was unconditionally querying `std::chrono::steady_clock::now()` and writing to the `last_fire_` unordered_map for every single event fired, even if the event had no throttle limit configured. This causes unnecessary syscalls and cache-invalidating map mutations on the hottest path of the engine.
**Action:** Always defer expensive time checks and state updates until after verifying they are required by the current configuration (e.g. only get time if event is actually throttled).

## 2024-10-24 - Unnecessary heap allocations on hot paths
**Learning:** `EventBus::fire` and `EventBus::fire_cancellable` were performing heap allocations via `std::make_unique` for every event fired, despite the internal dispatch mechanism not taking ownership of the event pointer. Since event buses are typically high-frequency, hot-path components, these continuous dynamic memory allocations caused unnecessary overhead.
**Action:** Always verify if an object passed by pointer or reference in a hot path actually requires dynamic allocation. If ownership is not transferred and the object's lifetime only needs to span the synchronous function call, use stack allocation to avoid heap overhead.
