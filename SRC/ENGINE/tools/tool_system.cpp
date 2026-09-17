#include "tools/tool_system.h"

#include <algorithm>

namespace q3d {
namespace tools {

void ITool::activate(ToolContext*) {}
void ITool::deactivate(ToolContext*) {}

void ITool::on_frame(ToolContext*, float) {}

bool ITool::on_mouse_move(ToolContext*, int, int, int) { return false; }
bool ITool::on_mouse_down(ToolContext*, int, int, int) { return false; }
bool ITool::on_mouse_up(ToolContext*, int, int, int) { return false; }
bool ITool::on_key_down(ToolContext*, int, int) { return false; }
bool ITool::on_key_up(ToolContext*, int, int) { return false; }
bool ITool::on_char(ToolContext*, int) { return false; }
bool ITool::on_scroll(ToolContext*, int, int) { return false; }

bool ITool::on_click(ToolContext*, const Ray&, const HitResult&) { return false; }
bool ITool::on_drag_start(ToolContext*, const Ray&, const HitResult&) { return false; }
bool ITool::on_drag_move(ToolContext*, const Ray&, const Vec3&) { return false; }
bool ITool::on_drag_end(ToolContext*) { return false; }

void ITool::apply_gizmo(ToolContext*, const Vec3&, GizmoMode) {}

void ITool::render_overlay(ToolContext*, Renderer*) {}

ToolContext::ToolContext(MovieHandle m, SceneHandle s, Viewport* v)
    : movie(m), scene(s), viewport(v), hovered_actor(nullptr), gizmo_position(), user_data(nullptr) {}

ActorHandle ToolContext::spawn_actor(const char* /*template_id*/)
{
    return nullptr;
}

void ToolContext::delete_selection()
{
    selected_actors.clear();
}

void ToolContext::select(ActorHandle a)
{
    if (!a) return;
    selected_actors.push_back(a);
}

void ToolContext::deselect(ActorHandle a)
{
    if (!a) return;
    auto it = std::find(selected_actors.begin(), selected_actors.end(), a);
    if (it != selected_actors.end())
        selected_actors.erase(it);
}

void ToolContext::select_all()
{
}

void ToolContext::deselect_all()
{
    selected_actors.clear();
}

void ToolContext::undo()
{
}

void ToolContext::redo()
{
}

Ray ToolContext::screen_to_ray(int, int) const
{
    Ray r;
    return r;
}

HitResult ToolContext::raycast(const Ray&) const
{
    HitResult h;
    return h;
}

void ToolContext::mark_dirty()
{
}

} // namespace tools
} // namespace q3d