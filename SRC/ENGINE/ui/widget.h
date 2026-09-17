#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "theme.h"

namespace q3d {
namespace ui {

struct Animation;

class Widget {
public:
    virtual ~Widget() = default;

    virtual void build();
    virtual void destroy();
    virtual void show();
    virtual void hide();
    virtual void set_enabled(bool v);

    virtual void measure(Vec2 available);
    virtual void arrange(Vec2 pos, Vec2 size);
    virtual Vec2 desired_size() const { return desired_; }

    virtual bool on_mouse_move(int x, int y, int buttons);
    virtual bool on_mouse_down(int x, int y, int button);
    virtual bool on_mouse_up(int x, int y, int button);
    virtual bool on_key_down(int key, int mods);
    virtual bool on_key_up(int key, int mods);
    virtual bool on_scroll(int dx, int dy);
    virtual bool on_char(int ch);
    virtual bool on_focus();
    virtual bool on_blur();

    void set_style(const Style& s) { style_ = s; }
    const Style& style() const { return style_; }
    void set_parent(Widget* p) { parent_ = p; }
    Widget* parent() const { return parent_; }

    virtual bool focusable() const { return false; }
    bool focused() const { return focused_; }
    void request_focus();

    void animate(const Animation& anim);
    void stop_animation();

    const std::string& id() const { return id_; }
    void set_id(const std::string& id) { id_ = id; }

protected:
    std::string id_;
    Widget* parent_ = nullptr;
    Vec2 pos_{};
    Vec2 size_{};
    Vec2 desired_{};
    Style style_{};
    bool visible_ = true;
    bool enabled_ = true;
    bool focused_ = false;
    std::vector<std::shared_ptr<Animation>> animations_;
};

class WidgetFactory {
public:
    using Creator = std::function<std::shared_ptr<Widget>()>;

    static WidgetFactory& instance();

    template<typename T>
    void register_type(const std::string& type_name) {
        creators_[type_name] = []() { return std::make_shared<T>(); };
        type_names_.insert(type_name);
    }

    std::shared_ptr<Widget> create(const std::string& type_name);

    bool has(const std::string& type_name) const;

    const std::unordered_set<std::string>& types() const { return type_names_; }

private:
    WidgetFactory() = default;

    std::unordered_map<std::string, Creator> creators_;
    std::unordered_set<std::string> type_names_;
};

class Panel;

class Button : public Widget {};
class Label : public Widget {};
class Image : public Widget {};
class Slider : public Widget {};
class ListView : public Widget {};
class TreeView : public Widget {};
class TextInput : public Widget {};
class ColorPicker : public Widget {};
class FilePicker : public Widget {};
class Dropdown : public Widget {};
class Tabs : public Widget {};
class Panel : public Widget {};
class Modal : public Widget {};
class Tooltip : public Widget {};
class MenuBar : public Widget {};
class ContextMenu : public Widget {};
class ScrollArea : public Widget {};
class SplitView : public Widget {};
class DockArea : public Widget {};
class ResizeHandle : public Widget {};

} // namespace ui
} // namespace q3d