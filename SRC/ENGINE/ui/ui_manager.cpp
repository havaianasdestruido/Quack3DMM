#include "ui_manager.h"

namespace q3d {
namespace ui {

UIManager& UIManager::instance() {
    static UIManager inst;
    return inst;
}

std::shared_ptr<Panel> UIManager::get_panel(const char* id) const {
    auto it = panels_.find(id ? id : "");
    if (it != panels_.end()) return it->second;
    return nullptr;
}

const std::unordered_map<std::string, std::shared_ptr<Panel>>& UIManager::panels() const {
    return panels_;
}

void UIManager::add_panel(std::shared_ptr<Panel> panel) {
    if (!panel) return;
    panels_[panel->id()] = panel;
}

void UIManager::remove_panel(const char* id) {
    panels_.erase(id ? id : "");
}

void UIManager::set_theme(const Theme& theme) {
    theme_ = theme;
}

void UIManager::load_theme(const char*) {}

void UIManager::set_locale(const std::string& locale) {
    locale_ = locale;
}

std::string UIManager::localize(const char* key) const {
    if (!key) return {};
    auto itLocale = translations_.find(locale_);
    if (itLocale == translations_.end()) return key;
    auto it = itLocale->second.find(key);
    if (it == itLocale->second.end()) return key;
    return it->second;
}

const std::unordered_map<std::string, std::string>& UIManager::current_translations() const {
    static std::unordered_map<std::string, std::string> empty;
    auto it = translations_.find(locale_);
    if (it == translations_.end()) return empty;
    return it->second;
}

void UIManager::load_translations(const std::string& locale, const char* data) {
    (void)data;
    translations_[locale];
}

void UIManager::reload_ui_scripts() {}
void UIManager::reload_theme() {}
void UIManager::reload_translations() {}

void UIManager::set_accessibility_mode(bool enabled) {
    accessibility_ = enabled;
}

void UIManager::set_high_contrast(bool) {}
void UIManager::set_large_text(bool) {}
void UIManager::set_screen_reader_enabled(bool) {}

void UIManager::set_cursor(CursorType type) {
    cursor_ = type;
}

void UIManager::set_debug_panels(bool v) {
    debug_panels_ = v;
}

void UIManager::show_inspector() {}
void UIManager::show_performance_overlay() {}

void UIManager::save_layout(const char*) {}
void UIManager::load_layout(const char*) {}
void UIManager::reset_layout() {}
void UIManager::rebuild_layout() { layout_dirty_ = false; }

} // namespace ui
} // namespace q3d
