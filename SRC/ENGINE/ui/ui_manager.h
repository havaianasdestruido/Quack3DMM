#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "theme.h"
#include "widget.h"

namespace q3d {
namespace ui {

enum class CursorType {
    Arrow,
    Hand,
    Text,
    Resize,
};

class UIManager {
public:
    static UIManager& instance();

    std::shared_ptr<Panel> get_panel(const char* id) const;
    const std::unordered_map<std::string, std::shared_ptr<Panel>>& panels() const;
    void add_panel(std::shared_ptr<Panel> panel);
    void remove_panel(const char* id);

    void set_theme(const Theme& theme);
    const Theme& theme() const { return theme_; }
    void load_theme(const char* vpath);

    void set_locale(const std::string& locale);
    std::string localize(const char* key) const;
    const std::unordered_map<std::string, std::string>& current_translations() const;
    void load_translations(const std::string& locale, const char* data);

    void reload_ui_scripts();
    void reload_theme();
    void reload_translations();

    void set_accessibility_mode(bool enabled);
    bool accessibility_mode() const { return accessibility_; }
    void set_high_contrast(bool enabled);
    void set_large_text(bool enabled);
    void set_screen_reader_enabled(bool enabled);

    void set_cursor(CursorType type);
    CursorType current_cursor() const { return cursor_; }

    void set_debug_panels(bool v);
    void show_inspector();
    void show_performance_overlay();

    void save_layout(const char* name);
    void load_layout(const char* name);
    void reset_layout();
    void set_layout_dirty() { layout_dirty_ = true; }
    void rebuild_layout();

private:
    UIManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Panel>> panels_;
    Theme theme_{};
    std::string locale_ = "en";
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> translations_;
    bool accessibility_ = false;
    CursorType cursor_ = CursorType::Arrow;
    bool debug_panels_ = false;
    bool layout_dirty_ = false;
};

} // namespace ui
} // namespace q3d
