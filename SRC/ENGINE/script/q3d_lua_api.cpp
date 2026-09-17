#include "q3d_lua_api.h"

#include <string>
#include <vector>

namespace q3d {
namespace script {

// -------------------------------------------------------------------
// q3d.actor
// -------------------------------------------------------------------

static int l_q3d_actor_create(lua_State* L) {
    const char* template_id = luaL_checkstring(L, 1);
    (void)template_id;
    lua_pushnil(L);
    return 1;
}

static int l_q3d_actor_find(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    (void)name;
    lua_pushnil(L);
    return 1;
}

static int l_q3d_actor_list(lua_State* L) {
    lua_newtable(L);
    return 1;
}

static int l_q3d_actor_set_position(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_actor_get_position(lua_State* L) {
    (void)L;
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 3;
}

static int l_q3d_actor_set_rotation(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_actor_set_scale(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_actor_play_animation(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_actor_destroy(lua_State* L) {
    (void)L;
    return 0;
}

static const luaL_Reg q3d_actor_funcs[] = {
    { "create", l_q3d_actor_create },
    { "find", l_q3d_actor_find },
    { "list", l_q3d_actor_list },
    { "set_position", l_q3d_actor_set_position },
    { "get_position", l_q3d_actor_get_position },
    { "set_rotation", l_q3d_actor_set_rotation },
    { "set_scale", l_q3d_actor_set_scale },
    { "play_animation", l_q3d_actor_play_animation },
    { "destroy", l_q3d_actor_destroy },
    { nullptr, nullptr }
};

// -------------------------------------------------------------------
// q3d.scene
// -------------------------------------------------------------------

static int l_q3d_scene_load(lua_State* L) {
    const char* path = luaL_optstring(L, 1, "");
    (void)path;
    lua_pushboolean(L, 0);
    return 1;
}

static int l_q3d_scene_save(lua_State* L) {
    const char* path = luaL_optstring(L, 1, "");
    (void)path;
    lua_pushboolean(L, 0);
    return 1;
}

static int l_q3d_scene_play(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_scene_stop(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_scene_set_frame(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_scene_get_frame(lua_State* L) {
    lua_pushinteger(L, 0);
    return 1;
}

static int l_q3d_scene_set_speed(lua_State* L) {
    (void)L;
    return 0;
}

static const luaL_Reg q3d_scene_funcs[] = {
    { "load", l_q3d_scene_load },
    { "save", l_q3d_scene_save },
    { "play", l_q3d_scene_play },
    { "stop", l_q3d_scene_stop },
    { "set_frame", l_q3d_scene_set_frame },
    { "get_frame", l_q3d_scene_get_frame },
    { "set_speed", l_q3d_scene_set_speed },
    { nullptr, nullptr }
};

// -------------------------------------------------------------------
// q3d.event
// -------------------------------------------------------------------

static int l_q3d_event_register(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_event_fire(lua_State* L) {
    (void)L;
    return 0;
}

static int l_q3d_event_cancel(lua_State* L) {
    (void)L;
    return 0;
}

static const luaL_Reg q3d_event_funcs[] = {
    { "register", l_q3d_event_register },
    { "fire", l_q3d_event_fire },
    { "cancel", l_q3d_event_cancel },
    { nullptr, nullptr }
};

// -------------------------------------------------------------------
// q3d.vfs
// -------------------------------------------------------------------

static int l_q3d_vfs_read(lua_State* L) {
    const char* vpath = luaL_checkstring(L, 1);
    (void)vpath;
    lua_pushstring(L, "");
    return 1;
}

static int l_q3d_vfs_exists(lua_State* L) {
    const char* vpath = luaL_checkstring(L, 1);
    (void)vpath;
    lua_pushboolean(L, 0);
    return 1;
}

static int l_q3d_vfs_write(lua_State* L) {
    (void)L;
    lua_pushboolean(L, 0);
    return 1;
}

static int l_q3d_vfs_list(lua_State* L) {
    lua_newtable(L);
    return 1;
}

static const luaL_Reg q3d_vfs_funcs[] = {
    { "read", l_q3d_vfs_read },
    { "exists", l_q3d_vfs_exists },
    { "write", l_q3d_vfs_write },
    { "list", l_q3d_vfs_list },
    { nullptr, nullptr }
};

// -------------------------------------------------------------------
// q3d.math
// -------------------------------------------------------------------

static int l_q3d_math_lerp(lua_State* L) {
    double a = luaL_checknumber(L, 1);
    double b = luaL_checknumber(L, 2);
    double t = luaL_checknumber(L, 3);
    lua_pushnumber(L, a + (b - a) * t);
    return 1;
}

static int l_q3d_math_vec3(lua_State* L) {
    lua_Number x = luaL_checknumber(L, 1);
    lua_Number y = luaL_checknumber(L, 2);
    lua_Number z = luaL_checknumber(L, 3);
    lua_createtable(L, 0, 3);
    lua_pushnumber(L, x); lua_setfield(L, -2, "x");
    lua_pushnumber(L, y); lua_setfield(L, -2, "y");
    lua_pushnumber(L, z); lua_setfield(L, -2, "z");
    return 1;
}

static int l_q3d_math_clamp(lua_State* L) {
    double v = luaL_checknumber(L, 1);
    double lo = luaL_checknumber(L, 2);
    double hi = luaL_checknumber(L, 3);
    lua_pushnumber(L, v < lo ? lo : (v > hi ? hi : v));
    return 1;
}

static const luaL_Reg q3d_math_funcs[] = {
    { "lerp", l_q3d_math_lerp },
    { "vec3", l_q3d_math_vec3 },
    { "clamp", l_q3d_math_clamp },
    { nullptr, nullptr }
};

// -------------------------------------------------------------------
// q3d.log
// -------------------------------------------------------------------

static int l_q3d_log_info(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    (void)msg;
    return 0;
}

static int l_q3d_log_warn(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    (void)msg;
    return 0;
}

static int l_q3d_log_error(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    (void)msg;
    return 0;
}

static const luaL_Reg q3d_log_funcs[] = {
    { "info", l_q3d_log_info },
    { "warn", l_q3d_log_warn },
    { "error", l_q3d_log_error },
    { nullptr, nullptr }
};

// -------------------------------------------------------------------
// q3d.ui
// -------------------------------------------------------------------

static int l_q3d_ui_panel(lua_State* L) {
    (void)L;
    lua_pushboolean(L, 0);
    return 1;
}

static int l_q3d_ui_button(lua_State* L) {
    (void)L;
    lua_pushboolean(L, 0);
    return 1;
}

static int l_q3d_ui_close_panel(lua_State* L) {
    (void)L;
    return 0;
}

static const luaL_Reg q3d_ui_funcs[] = {
    { "panel", l_q3d_ui_panel },
    { "button", l_q3d_ui_button },
    { "close_panel", l_q3d_ui_close_panel },
    { nullptr, nullptr }
};

// -------------------------------------------------------------------
// Entry point
// -------------------------------------------------------------------

void q3d_lua_api_expose(lua_State* L) {
    lua_newtable(L);

    lua_newtable(L);
    luaL_setfuncs(L, q3d_actor_funcs, 0);
    lua_setfield(L, -2, "actor");

    lua_newtable(L);
    luaL_setfuncs(L, q3d_scene_funcs, 0);
    lua_setfield(L, -2, "scene");

    lua_newtable(L);
    luaL_setfuncs(L, q3d_event_funcs, 0);
    lua_setfield(L, -2, "event");

    lua_newtable(L);
    luaL_setfuncs(L, q3d_vfs_funcs, 0);
    lua_setfield(L, -2, "vfs");

    lua_newtable(L);
    luaL_setfuncs(L, q3d_math_funcs, 0);
    lua_setfield(L, -2, "math");

    lua_newtable(L);
    luaL_setfuncs(L, q3d_log_funcs, 0);
    lua_setfield(L, -2, "log");

    lua_newtable(L);
    luaL_setfuncs(L, q3d_ui_funcs, 0);
    lua_setfield(L, -2, "ui");

    lua_setglobal(L, "q3d");
}

} // namespace script
} // namespace q3d