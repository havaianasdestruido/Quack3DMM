#include "tools/tool_manager.h"

#include <algorithm>

namespace q3d {
namespace tools {

ToolManager& ToolManager::instance()
{
    static ToolManager mgr;
    return mgr;
}

void ToolManager::register_tool(shared_ptr<ITool> tool)
{
    if (!tool)
        return;
    for (auto& t : tools_)
    {
        if (t->id() == tool->id())
            return;
    }
    tools_.push_back(tool);
}

void ToolManager::unregister_tool(const char* tool_id)
{
    if (!tool_id)
        return;
    string id(tool_id);
    tools_.erase(std::remove_if(tools_.begin(), tools_.end(), [&](const shared_ptr<ITool>& t) { return t->id() == id; }), tools_.end());
}

void ToolManager::unregister_all_for_mod(const char*)
{
}

shared_ptr<ITool> ToolManager::get_tool(const char* id) const
{
    if (!id)
        return nullptr;
    string sid(id);
    for (auto& t : tools_)
    {
        if (t->id() == sid)
            return t;
    }
    return nullptr;
}

void ToolManager::activate(const char* tool_id)
{
    auto t = get_tool(tool_id);
    if (!t)
        return;
    if (current_tool_ && current_tool_ != t)
        current_tool_->deactivate(nullptr);
    current_tool_ = t;
    current_tool_->activate(nullptr);
}

void ToolManager::activate_default()
{
    for (auto& t : tools_)
    {
        if (current_tool_ && current_tool_ != t)
            current_tool_->deactivate(nullptr);
        current_tool_ = t;
        current_tool_->activate(nullptr);
        break;
    }
}

void ToolManager::deactivate()
{
    if (current_tool_)
        current_tool_->deactivate(nullptr);
    current_tool_.reset();
}

vector<shared_ptr<ITool>> ToolManager::get_by_category(ToolCategory) const
{
    return tools_;
}

vector<shared_ptr<ITool>> ToolManager::get_by_category(const string& cat) const
{
    vector<shared_ptr<ITool>> out;
    for (auto& t : tools_)
    {
        if (t->category() == cat)
            out.push_back(t);
    }
    return out;
}

void ToolManager::register_hotkey(const char* tool_id, int vk, int)
{
    if (!tool_id)
        return;
    string id(tool_id);
    auto it = reverse_hotkey_.find(vk);
    if (it != reverse_hotkey_.end())
    {
        if (hotkey_conflict_handler_)
            hotkey_conflict_handler_(tool_id, vk, it->second.c_str());
        return;
    }
    hotkey_map_[id] = vk;
    reverse_hotkey_[vk] = id;
}

void ToolManager::unregister_hotkey(const char* tool_id)
{
    if (!tool_id)
        return;
    string id(tool_id);
    auto it = hotkey_map_.find(id);
    if (it == hotkey_map_.end())
        return;
    int vk = it->second;
    hotkey_map_.erase(it);
    reverse_hotkey_.erase(vk);
}

shared_ptr<ITool> ToolManager::tool_for_hotkey(int vk, int) const
{
    auto it = reverse_hotkey_.find(vk);
    if (it == reverse_hotkey_.end())
        return nullptr;
    string id = it->second;
    for (auto& t : tools_)
    {
        if (t->id() == id)
            return t;
    }
    return nullptr;
}

void ToolManager::set_hotkey_conflict_handler(function<void(const char*, int, const char*)> handler)
{
    hotkey_conflict_handler_ = std::move(handler);
}

vector<shared_ptr<ITool>> ToolManager::palette_tools() const
{
    vector<shared_ptr<ITool>> out;
    for (auto& id : palette_)
    {
        for (auto& t : tools_)
        {
            if (t->id() == id)
                out.push_back(t);
        }
    }
    return out;
}

void ToolManager::set_palette_tools(const vector<string>& tool_ids)
{
    palette_ = tool_ids;
}

void ToolManager::save_palette(const char*)
{
}

void ToolManager::load_palette(const char*)
{
}

void ToolManager::register_context_menu(const char* menu_id, ContextMenuBuilder builder)
{
    if (!menu_id)
        return;
    context_menus_[menu_id] = std::move(builder);
}

void ToolManager::show_context_menu(const char* menu_id, ToolContext* ctx, int x, int y)
{
    if (!menu_id)
        return;
    auto it = context_menus_.find(menu_id);
    if (it == context_menus_.end())
        return;
    if (it->second)
        it->second(ctx, x, y);
}

vector<CommandPaletteEntry> ToolManager::command_palette_entries() const
{
    vector<CommandPaletteEntry> out;
    for (auto& t : tools_)
    {
        CommandPaletteEntry e;
        e.id = t->id();
        e.label = t->label();
        e.category = t->category();
        out.push_back(e);
    }
    return out;
}

void ToolManager::execute_command(const char* command_id)
{
    activate(command_id);
}

string ToolManager::get_tooltip(const char* tool_id) const
{
    if (!tool_id)
        return string();
    auto it = tooltips_.find(string(tool_id));
    if (it != tooltips_.end())
        return it->second;
    return string();
}

void ToolManager::set_tooltip(const char* tool_id, const string& tip)
{
    if (tool_id)
        tooltips_[string(tool_id)] = tip;
}

void ToolManager::on_frame(float dt)
{
    if (!current_tool_)
        return;
    current_tool_->on_frame(nullptr, dt);
}

void ToolManager::on_mouse_move(int x, int y, int buttons)
{
    if (!current_tool_)
        return;
    current_tool_->on_mouse_move(nullptr, x, y, buttons);
}

void ToolManager::on_mouse_down(int x, int y, int button)
{
    if (!current_tool_)
        return;
    current_tool_->on_mouse_down(nullptr, x, y, button);
}

void ToolManager::on_mouse_up(int x, int y, int button)
{
    if (!current_tool_)
        return;
    current_tool_->on_mouse_up(nullptr, x, y, button);
}

void ToolManager::on_key_down(int key, int mods)
{
    auto t = tool_for_hotkey(key, mods);
    if (t)
    {
        if (current_tool_ && current_tool_ != t)
            current_tool_->deactivate(nullptr);
        current_tool_ = t;
        current_tool_->activate(nullptr);
        return;
    }
    if (current_tool_)
        current_tool_->on_key_down(nullptr, key, mods);
}

void ToolManager::on_key_up(int key, int mods)
{
    if (!current_tool_)
        return;
    current_tool_->on_key_up(nullptr, key, mods);
}

void ToolManager::on_scroll(int dx, int dy)
{
    if (!current_tool_)
        return;
    current_tool_->on_scroll(nullptr, dx, dy);
}

} // namespace tools
} // namespace q3d