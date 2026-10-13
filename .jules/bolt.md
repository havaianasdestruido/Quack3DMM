## 2024-04-14 - Optimize insertion and deletion in event handlers and lists
**Learning:** Avoid full O(N log N) `std::sort` for inserting single elements into previously sorted lists when priority insertion is used (e.g., `EventBus`, `HookSystem`, `RenderPassManager`, `VFS`). In addition, `std::remove_if` visits the entire vector even when deleting a unique element.
**Action:** Use an O(N) bubble-up insertion step instead of full `std::sort`, and replace `std::remove_if` with `std::find_if` + `erase` to return early.
