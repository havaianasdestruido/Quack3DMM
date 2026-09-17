#include "lua_engine.h"

#include "q3d_lua_api.h"
#include "script_manager.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace q3d {
namespace script {

// Lua state RAII deleter
static void lua_state_deleter(lua_State* L) noexcept {
    if (L) {
        lua_close(L);
    }
}

LuaVM::LuaVM() : _L(nullptr, lua_state_deleter), _modId(), _running(false) {}

LuaVM::~LuaVM() = default;

void LuaVM::_reset() {
    _L.reset();
    _running = false;
}

[[nodiscard]] bool LuaVM::_load_file(const char* vpath) noexcept {
    FILE* f = fopen(vpath, "rb");
    if (!f) {
        return false;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (len <= 0) {
        fclose(f);
        return false;
    }

    std::string buf(static_cast<size_t>(len) + 1, '\0');
    size_t n = fread(buf.data(), 1, static_cast<size_t>(len), f);
    fclose(f);

    if (n != static_cast<size_t>(len)) {
        return false;
    }
    buf[len] = '\0';

    if (luaL_loadstring(_L.get(), buf.c_str()) != LUA_OK) {
        return false;
    }

    return true;
}

[[nodiscard]] bool LuaVM::do_file(const char* vpath) noexcept {
    if (!_L) {
        return false;
    }

    if (!_load_file(vpath)) {
        return false;
    }

    if (lua_pcall(_L.get(), 0, LUA_MULTRET, 0) != LUA_OK) {
        return false;
    }

    return true;
}

[[nodiscard]] bool LuaVM::do_string(const char* code, const char* chunkname) noexcept {
    if (!_L || !code) {
        return false;
    }
    (void)chunkname;

    if (luaL_loadstring(_L.get(), code) != LUA_OK) {
        return false;
    }

    if (lua_pcall(_L.get(), 0, LUA_MULTRET, 0) != LUA_OK) {
        return false;
    }

    return true;
}

void LuaVM::expose_api() {
    if (!_L) {
        return;
    }

    q3d_lua_api_expose(_L.get());
    luaL_openlibs(_L.get());
}

void LuaVM::reload() {
    _reset();

    _L.reset(luaL_newstate());

    if (!_L) {
        return;
    }

    expose_api();
}

void LuaVM::gc(int target_kb) noexcept {
    if (!_L) {
        return;
    }

    if (target_kb <= 0) {
        lua_gc(_L.get(), LUA_GCCOLLECT, 0);
        return;
    }

    int64_t before_kb = static_cast<int64_t>(lua_gc(_L.get(), LUA_GCCOLLECT, 0));
    (void)before_kb;

    int64_t threshold_kb = 0;
    while (threshold_kb < target_kb) {
        int state = lua_gc(_L.get(), LUA_GCSTEP, 1);
        int64_t after_kb = static_cast<int64_t>(lua_gc(_L.get(), LUA_GCCOUNT, 0));
        threshold_kb = after_kb;
        if (state) {
            break;
        }
    }
}

// -------------------------------------------------------------------
// LuaHost
// -------------------------------------------------------------------

LuaHost& LuaHost::instance() {
    static LuaHost host;
    return host;
}

[[nodiscard]] std::shared_ptr<LuaVM> LuaHost::create_vm(const char* mod_id) {
    auto vm = std::make_shared<LuaVM>();
    vm->_L.reset(luaL_newstate());

    if (!vm->_L) {
        return nullptr;
    }

    vm->_modId = mod_id ? std::string(mod_id) : "default";
    vm->_running = true;
    vm->expose_api();

    _vms.push_back(vm);
    return vm;
}

void LuaHost::tick(float dt) noexcept {
    for (auto& vm : _vms) {
        if (vm && vm->_running) {
            lua_State* L = vm->_L.get();
            if (!L) continue;

            lua_getglobal(L, "q3d");
            if (lua_istable(L, -1)) {
                lua_getfield(L, -1, "_tick");
                if (lua_isfunction(L, -1)) {
                    lua_pushnumber(L, static_cast<lua_Number>(dt));
                    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                        // silently ignore tick errors
                    }
                }
            }
            lua_pop(L, 1);
        }
    }
}

void LuaHost::check_and_reload() {
    for (auto& vm : _vms) {
        if (!vm) continue;
        (void)_fileMtimes;
    }
}

void LuaHost::register_module(const char* name, const luaL_Reg* funcs) noexcept {
    if (name && funcs) {
        _modules[name] = funcs;
    }
}

[[nodiscard]] std::string LuaHost::evaluate(const char* expr) {
    if (!expr) return "";

    std::string result;
    for (auto& vm : _vms) {
        if (!vm || !vm->_running || !vm->_L) continue;

        lua_State* L = vm->_L.get();
        if (luaL_loadstring(L, expr) != LUA_OK) {
            continue;
        }

        if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
            continue;
        }

        if (lua_isstring(L, -1)) {
            result = lua_tostring(L, -1);
        } else if (lua_isnumber(L, -1)) {
            result = std::to_string(lua_tonumber(L, -1));
        } else if (lua_isboolean(L, -1)) {
            result = lua_toboolean(L, -1) ? "true" : "false";
        }
        lua_pop(L, 1);
        break; // use first active VM
    }
    return result;
}

[[nodiscard]] std::vector<std::pair<std::string, std::string>> LuaHost::list_scripts() const {
    std::vector<std::pair<std::string, std::string>> scripts;
    for (const auto& vm : _vms) {
        if (vm) {
            scripts.emplace_back(vm->_modId, "active");
        }
    }
    return scripts;
}

} // namespace script
} // namespace q3d