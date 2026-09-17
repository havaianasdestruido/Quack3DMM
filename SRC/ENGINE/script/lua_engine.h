#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

extern "C" {
#include <lua.hpp>
}

namespace q3d {
namespace script {

class LuaVM;

class LuaHost;

class LuaVM {
public:
    LuaVM();
    ~LuaVM();

    friend class LuaHost;

    [[nodiscard]] bool do_file(const char* vpath) noexcept;
    [[nodiscard]] bool do_string(const char* code, const char* chunkname = "=") noexcept;

    template<typename... Args>
    [[nodiscard]] int call(const char* func, Args... args) noexcept;

    void expose_api();
    void reload();
    void gc(int target_kb = 0) noexcept;

    [[nodiscard]] lua_State* L() noexcept { return _L.get(); }
    [[nodiscard]] const std::string& mod_id() const noexcept { return _modId; }

private:
    using LuaStatePtr = std::unique_ptr<lua_State, void(*)(lua_State*)>;

    LuaStatePtr _L;
    std::string _modId;
    bool _running;

    void _reset();
    [[nodiscard]] bool _load_file(const char* vpath) noexcept;
};

class LuaHost {
public:
    static LuaHost& instance();

    [[nodiscard]] std::shared_ptr<LuaVM> create_vm(const char* mod_id);

    void tick(float dt) noexcept;
    void check_and_reload();

    template<typename... Args>
    void dispatch_event(const char* event_name, Args... args);

    void register_module(const char* name, const luaL_Reg* funcs) noexcept;

    [[nodiscard]] std::string evaluate(const char* expr);

    [[nodiscard]] std::vector<std::pair<std::string, std::string>> list_scripts() const;

private:
    std::vector<std::shared_ptr<LuaVM>> _vms;
    std::unordered_map<std::string, long> _fileMtimes;
    std::unordered_map<std::string, const luaL_Reg*> _modules;
};

} // namespace script
} // namespace q3d

#include "lua_engine.inl"