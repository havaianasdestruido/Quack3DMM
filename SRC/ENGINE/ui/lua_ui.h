#pragma once

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

namespace q3d {
namespace ui {
namespace lua {
namespace api {

int l_panel_new(lua_State* L);
int l_panel_add(lua_State* L);
int l_panel_remove(lua_State* L);
int l_panel_clear(lua_State* L);
int l_panel_set_title(lua_State* L);
int l_panel_set_pos(lua_State* L);
int l_panel_set_size(lua_State* L);
int l_panel_show(lua_State* L);
int l_panel_hide(lua_State* L);
int l_panel_dock(lua_State* L);

int l_button(lua_State* L);
int l_label(lua_State* L);
int l_image(lua_State* L);
int l_slider(lua_State* L);
int l_list(lua_State* L);
int l_tree(lua_State* L);
int l_text_input(lua_State* L);
int l_color_picker(lua_State* L);
int l_file_picker(lua_State* L);
int l_dropdown(lua_State* L);
int l_tabs(lua_State* L);

int l_widget_set_pos(lua_State* L);
int l_widget_set_size(lua_State* L);
int l_widget_set_enabled(lua_State* L);
int l_widget_set_visible(lua_State* L);
int l_widget_set_style(lua_State* L);
int l_widget_on_click(lua_State* L);
int l_widget_on_change(lua_State* L);
int l_widget_on_hover(lua_State* L);

int l_layout_hbox(lua_State* L);
int l_layout_vbox(lua_State* L);
int l_layout_grid(lua_State* L);
int l_layout_dock(lua_State* L);

int l_theme_load(lua_State* L);
int l_theme_set_color(lua_State* L);
int l_theme_set_font(lua_State* L);

extern const luaL_Reg ui_functions[];

} // namespace api
} // namespace lua
} // namespace ui
} // namespace q3d
