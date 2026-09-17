#pragma once

#include "tools/tool_system.h"

#include <functional>
#include <unordered_map>
#include <map>

namespace q3d {
namespace tools {

using std::function;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;
using std::map;

struct CommandPaletteEntry
{
    string id;
    string label;
    string category;
};

using ContextMenuBuilder = function<void(ToolContext*, int, int)>;

class ToolManager
{
public:
    static ToolManager& instance();

    void register_tool(shared_ptr<ITool> tool);
    void unregister_tool(const char* tool_id);
    void unregister_all_for_mod(const char* mod_id);

    shared_ptr<ITool> get_tool(const char* id) const;
    const vector<shared_ptr<ITool>>& all_tools() const { return tools_; }

    void activate(const char* tool_id);
    void activate_default();
    void deactivate();

    shared_ptr<ITool> current_tool() const { return current_tool_; }
    bool has_tool() const { return current_tool_ != nullptr; }

    vector<shared_ptr<ITool>> get_by_category(ToolCategory cat) const;
    vector<shared_ptr<ITool>> get_by_category(const string& cat) const;

    void register_hotkey(const char* tool_id, int vk, int mods);
    void unregister_hotkey(const char* tool_id);
    shared_ptr<ITool> tool_for_hotkey(int vk, int mods) const;
    void set_hotkey_conflict_handler(function<void(const char*, int, const char*)> handler);

    vector<shared_ptr<ITool>> palette_tools() const;
    void set_palette_tools(const vector<string>& tool_ids);
    void save_palette(const char* name);
    void load_palette(const char* name);

    void register_context_menu(const char* menu_id, ContextMenuBuilder builder);
    void show_context_menu(const char* menu_id, ToolContext* ctx, int x, int y);

    vector<CommandPaletteEntry> command_palette_entries() const;
    void execute_command(const char* command_id);

    string get_tooltip(const char* tool_id) const;
    void set_tooltip(const char* tool_id, const string& tip);

    void on_frame(float dt);
    void on_mouse_move(int x, int y, int buttons);
    void on_mouse_down(int x, int y, int button);
    void on_mouse_up(int x, int y, int button);
    void on_key_down(int key, int mods);
    void on_key_up(int key, int mods);
    void on_scroll(int dx, int dy);

private:
    ToolManager() = default;

    vector<shared_ptr<ITool>> tools_;
    shared_ptr<ITool> current_tool_;
    map<string, int> hotkey_map_;
    map<int, string> reverse_hotkey_;
    function<void(const char*, int, const char*)> hotkey_conflict_handler_;
    unordered_map<string, ContextMenuBuilder> context_menus_;
    map<string, string> tooltips_;
    vector<string> palette_;
};

} // namespace tools
} // namespace q3d