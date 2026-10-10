## 2024-05-24 - EventBus unnecessary overhead
**Learning:** The `EventBus::fire_internal` method was unconditionally querying `std::chrono::steady_clock::now()` and writing to the `last_fire_` unordered_map for every single event fired, even if the event had no throttle limit configured. This causes unnecessary syscalls and cache-invalidating map mutations on the hottest path of the engine.
**Action:** Always defer expensive time checks and state updates until after verifying they are required by the current configuration (e.g. only get time if event is actually throttled).
