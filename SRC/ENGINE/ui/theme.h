#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace q3d {
namespace ui {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
    Vec2() = default;
    Vec2(float x_, float y_) : x(x_), y(y_) {}
};

struct Color {
    float r, g, b, a;

    static Color rgb(float r, float g, float b) { return {r, g, b, 1.0f}; }
    static Color rgba(float r, float g, float b, float a) { return {r, g, b, a}; }
    static Color hex(const char* hex);
};

struct Font {
    std::string family;
    int size;
    int weight;
    bool italic;

    static Font default_font() { return {"Segoe UI", 14, 400, false}; }
};

struct Style {
    Color bg_color = Color::rgb(0.15f, 0.15f, 0.15f);
    Color fg_color = Color::rgb(0.9f, 0.9f, 0.9f);
    Color border_color = Color::rgb(0.3f, 0.3f, 0.3f);
    Color hover_bg = Color::rgb(0.2f, 0.2f, 0.2f);
    Color active_bg = Color::rgb(0.3f, 0.5f, 0.8f);
    Color disabled_fg = Color::rgb(0.4f, 0.4f, 0.4f);
    float border_radius = 4.0f;
    float padding = 8.0f;
    Font font = Font::default_font();
    Vec2 margin = {0, 0};
    Vec2 spacing = {4, 4};
};

struct Theme {
    std::string name;
    Style window;
    Style button;
    Style button_hover;
    Style button_active;
    Style text_input;
    Style label;
    Style panel;
    Style menu;
    Style tooltip;
    float ui_scale = 1.0f;
};

} // namespace ui
} // namespace q3d