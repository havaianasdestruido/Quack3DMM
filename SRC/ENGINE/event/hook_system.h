#pragma once

#include <string>
#include <vector>
#include <map>
#include <string_view>
#include <functional>

namespace q3d {
namespace event {

using HookCallback = std::function<void()>;

class HookSystem {
public:
    static HookSystem &instance();

    // Performance optimization: Using transparent lookups to avoid allocating std::string on hook calls
    template<typename R, typename... Args>
    R call_hook(const char *name, R(*default_fn)(Args...), Args... args) {
        bool cancelled = false;
        if (auto *before = get_before_hooks(name)) {
            for (auto &entry : *before) {
                entry.second(name, &cancelled);
            }
        }

        R result{};
        if (!cancelled) {
            if (auto *replace = get_replace_hooks(name); replace && !replace->empty()) {
                result = replace->front()(name, args...);
            } else {
                result = default_fn(args...);
            }
        }

        if (auto *after = get_after_hooks(name)) {
            for (auto &entry : *after) {
                entry.second(name, result, args...);
            }
        }

        return result;
    }

    void register_before_hook(const char *mod_id, const char *name, HookCallback cb, int priority = 0);
    void register_after_hook(const char *mod_id, const char *name, HookCallback cb, int priority = 0);
    void register_replace_hook(const char *mod_id, const char *name, HookCallback cb);

    void unregister_mod_hooks(const char *mod_id);

    struct HookPoint {
        std::string name;
        std::string description;
        std::string signature;
        std::vector<std::string> mods_using;
    };

    std::vector<HookPoint> list_hook_points() const;

private:
    using HookEntry = std::pair<int, HookCallback>;

    const std::vector<HookEntry> *get_before_hooks(std::string_view name);
    const std::vector<HookEntry> *get_after_hooks(std::string_view name);
    const std::vector<HookCallback> *get_replace_hooks(std::string_view name);

    std::map<std::string, std::vector<HookEntry>, std::less<>> before_hooks_;
    std::map<std::string, std::vector<HookEntry>, std::less<>> after_hooks_;
    std::map<std::string, std::vector<HookCallback>, std::less<>> replace_hooks_;
    std::unordered_map<void *, std::string> hook_owners_;
};

} // namespace event
} // namespace q3d
