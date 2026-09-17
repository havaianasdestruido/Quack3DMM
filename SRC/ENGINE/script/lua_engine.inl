// lua_engine.inl - template implementations

#include "lua_engine.h"

namespace q3d {
namespace script {

// -------------------------------------------------------------------
// Argument push helpers
// -------------------------------------------------------------------

inline void push_arg(lua_State* L, bool v) noexcept { lua_pushboolean(L, v ? 1 : 0); }
inline void push_arg(lua_State* L, int v) noexcept { lua_pushinteger(L, v); }
inline void push_arg(lua_State* L, long v) noexcept { lua_pushinteger(L, v); }
inline void push_arg(lua_State* L, float v) noexcept { lua_pushnumber(L, v); }
inline void push_arg(lua_State* L, double v) noexcept { lua_pushnumber(L, v); }
inline void push_arg(lua_State* L, const char* v) noexcept { lua_pushstring(L, v); }
inline void push_arg(lua_State* L, const std::string& v) noexcept { lua_pushstring(L, v.c_str()); }
inline void push_arg(lua_State* L, void* v) noexcept { lua_pushlightuserdata(L, v); }

template<size_t N>
inline void push_arg(lua_State* L, const char (&v)[N]) noexcept { lua_pushlstring(L, v, N - 1); }

// -------------------------------------------------------------------
// LuaVM::call implementation
// -------------------------------------------------------------------

template<typename... Args>
[[nodiscard]] int LuaVM::call(const char* func, Args... args) noexcept {
    lua_State* L = _L.get();
    if (!L || !func) {
        return -1;
    }

    lua_getglobal(L, func);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return -1;
    }

    // Push arguments
    (push_arg(L, args), ...);

    constexpr int nargs = static_cast<int>(sizeof...(Args));
    if (lua_pcall(L, nargs, LUA_MULTRET, 0) != LUA_OK) {
        return -1;
    }

    int nresults = lua_gettop(L);
    return nresults;
}

// -------------------------------------------------------------------
// LuaHost::dispatch_event implementation
// -------------------------------------------------------------------

inline void dispatch_event_args(lua_State*) noexcept {}

template<typename T, typename... Rest>
inline void dispatch_event_args(lua_State* L, T arg, Rest... rest) noexcept {
    push_arg(L, arg);
    dispatch_event_args(L, rest...);
}

template<typename... Args>
void LuaHost::dispatch_event(const char* event_name, Args... args) {
    if (!event_name) return;

    for (auto& vm : _vms) {
        if (!vm || !vm->_running || !vm->_L) continue;

        lua_State* L = vm->_L.get();

        lua_getglobal(L, "q3d");
        if (!lua_istable(L, -1)) { lua_pop(L, 1); continue; }

        lua_getfield(L, -1, "event");
        if (!lua_istable(L, -1)) { lua_pop(L, 2); continue; }

        lua_getfield(L, -1, "_dispatch");
        if (!lua_isfunction(L, -1)) { lua_pop(L, 3); continue; }

        lua_pushstring(L, event_name);
        dispatch_event_args(L, args...);

        constexpr int nargs = 1 + static_cast<int>(sizeof...(Args));
        lua_pcall(L, nargs, 0, 0);
        lua_pop(L, 1);
    }
}

} // namespace script
} // namespace q3d
