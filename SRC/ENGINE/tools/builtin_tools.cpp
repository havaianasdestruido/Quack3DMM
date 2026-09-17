#include "tools/builtin_tools.h"
#include "tools/tool_manager.h"

namespace q3d {
namespace tools {

void SelectTool::activate(ToolContext*)
{
}

bool SelectTool::on_mouse_down(ToolContext* ctx, int, int, int button)
{
    if (button != 0)
        return false;
    if (ctx && ctx->hovered_actor)
        ctx->select(ctx->hovered_actor);
    return true;
}

void register_builtin_tools()
{
    auto& mgr = ToolManager::instance();
    mgr.register_tool(std::make_shared<SelectTool>());
    mgr.register_tool(std::make_shared<MoveTool>());
    mgr.register_tool(std::make_shared<RotateTool>());
    mgr.register_tool(std::make_shared<ScaleTool>());
    mgr.register_tool(std::make_shared<OrbitTool>());
    mgr.register_tool(std::make_shared<PanTool>());
    mgr.register_tool(std::make_shared<ZoomTool>());
    mgr.register_tool(std::make_shared<SpawnTool>());
    mgr.register_tool(std::make_shared<PlaybackTool>());
    mgr.register_tool(std::make_shared<EraserTool>());
}

} // namespace tools
} // namespace q3d