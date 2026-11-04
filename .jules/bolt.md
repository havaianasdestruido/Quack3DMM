## 2024-04-14 - Optimize insertion and deletion in event handlers and lists
**Learning:** Avoid full O(N log N) `std::sort` for inserting single elements into previously sorted lists when priority insertion is used (e.g., `EventBus`, `HookSystem`, `RenderPassManager`, `VFS`). In addition, `std::remove_if` visits the entire vector even when deleting a unique element.
**Action:** Use an O(N) bubble-up insertion step instead of full `std::sort`, and replace `std::remove_if` with `std::find_if` + `erase` to return early.
## 2024-05-24 - EventBus unnecessary overhead
**Learning:** The `EventBus::fire_internal` method was unconditionally querying `std::chrono::steady_clock::now()` and writing to the `last_fire_` unordered_map for every single event fired, even if the event had no throttle limit configured. This causes unnecessary syscalls and cache-invalidating map mutations on the hottest path of the engine.
**Action:** Always defer expensive time checks and state updates until after verifying they are required by the current configuration (e.g. only get time if event is actually throttled).

## 2024-10-24 - Unnecessary heap allocations on hot paths
**Learning:** `EventBus::fire` and `EventBus::fire_cancellable` were performing heap allocations via `std::make_unique` for every event fired, despite the internal dispatch mechanism not taking ownership of the event pointer. Since event buses are typically high-frequency, hot-path components, these continuous dynamic memory allocations caused unnecessary overhead.
**Action:** Always verify if an object passed by pointer or reference in a hot path actually requires dynamic allocation. If ownership is not transferred and the object's lifetime only needs to span the synchronous function call, use stack allocation to avoid heap overhead.
## 2024-10-24 - Unnecessary heap allocations on hot paths (String comparisons)
**Learning:** `ToolManager::unregister_tool` and `ToolManager::get_tool` were constructing temporary `std::string` objects from `const char*` purely for comparison against `ITool::id()`. This caused unnecessary heap allocations (unless SSO kicks in, but it's still overhead) when searching for tools. `std::string` provides an overloaded `operator==` for comparing against `const char*`.
**Action:** Avoid constructing temporary `std::string` instances for simple string comparison operations.
## 2024-10-25 - Unnecessary heap allocations when comparing IDs
**Learning:** `ITool::id()` returned a `std::string` by value, causing heap allocations every time it was called (like in loops when searching for tools).
**Action:** Use `std::string_view` for string return types and comparisons where ownership is not needed to prevent temporary string allocations.
## 2024-10-31 - Hoist strlen from loops
**Learning:** Comparing `std::string_view` against a `const char*` inside a loop will call `strlen` on the `const char*` every iteration.
**Action:** Convert `const char*` to `std::string_view` before the loop.
## 2024-10-31 - Zero-allocation map lookup with string_view
**Learning:** `std::map` and `std::unordered_map` with `std::string` keys require constructing a `std::string` when calling `.find()` or `.erase()` with a `const char*` or `std::string_view` parameter unless a transparent comparator (`std::less<>`) is provided. This causes unnecessary heap allocations.
**Action:** When using associative containers with string keys in C++14/17, explicitly define the comparator as `std::less<>` (e.g., `std::map<std::string, T, std::less<>>`) to enable zero-allocation lookups with `std::string_view` or `const char*`.
