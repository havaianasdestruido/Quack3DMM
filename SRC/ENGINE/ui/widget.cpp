#include "widget.h"

namespace q3d {
namespace ui {

void Widget::destroy() {}
void Widget::show() { visible_ = true; }
void Widget::hide() { visible_ = false; }
void Widget::set_enabled(bool v) { enabled_ = v; }

void Widget::build() {}
void Widget::measure(Vec2 available) { if (desired_.x == 0.0f) desired_.x = 100.0f; if (desired_.y == 0.0f) desired_.y = 24.0f; (void)available; }
void Widget::arrange(Vec2 pos, Vec2 size) { pos_ = pos; size_ = size; }

bool Widget::on_mouse_move(int, int, int) { return false; }
bool Widget::on_mouse_down(int, int, int) { return false; }
bool Widget::on_mouse_up(int, int, int) { return false; }
bool Widget::on_key_down(int, int) { return false; }
bool Widget::on_key_up(int, int) { return false; }
bool Widget::on_scroll(int, int) { return false; }
bool Widget::on_char(int) { return false; }
bool Widget::on_focus() { focused_ = true; return true; }
bool Widget::on_blur() { focused_ = false; return true; }

void Widget::request_focus() { focused_ = true; }

void Widget::animate(const Animation&) {}
void Widget::stop_animation() { animations_.clear(); }

WidgetFactory& WidgetFactory::instance() {
    static WidgetFactory inst;
    return inst;
}

std::shared_ptr<Widget> WidgetFactory::create(const std::string& type_name) {
    auto it = creators_.find(type_name);
    if (it != creators_.end()) return it->second();
    return nullptr;
}

bool WidgetFactory::has(const std::string& type_name) const {
    return creators_.find(type_name) != creators_.end();
}

} // namespace ui
} // namespace q3d
