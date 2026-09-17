#pragma once

#include "tools/tool_system.h"

namespace q3d {
namespace tools {

class SelectTool : public ITool
{
public:
    string id() const override { return "select"; }
    string label() const override { return "Select"; }
    string description() const override { return "Select actors"; }
    string category() const override { return "Select"; }
    string icon() const override { return "icons/select"; }
    int default_hotkey() const override { return 'Q'; }

    void activate(ToolContext* ctx) override;
    bool on_mouse_down(ToolContext* ctx, int x, int y, int button) override;
};

class MoveTool : public ITool
{
public:
    string id() const override { return "move"; }
    string label() const override { return "Move"; }
    string description() const override { return "Move actors"; }
    string category() const override { return "Modeling"; }
    string icon() const override { return "icons/move"; }
    int default_hotkey() const override { return 'W'; }
    bool has_gizmo() const override { return true; }
    GizmoMode gizmo_mode() const override { return GizmoMode::Translate; }
};

class RotateTool : public ITool
{
public:
    string id() const override { return "rotate"; }
    string label() const override { return "Rotate"; }
    string description() const override { return "Rotate actors"; }
    string category() const override { return "Modeling"; }
    string icon() const override { return "icons/rotate"; }
    int default_hotkey() const override { return 'E'; }
    bool has_gizmo() const override { return true; }
    GizmoMode gizmo_mode() const override { return GizmoMode::Rotate; }
};

class ScaleTool : public ITool
{
public:
    string id() const override { return "scale"; }
    string label() const override { return "Scale"; }
    string description() const override { return "Scale actors"; }
    string category() const override { return "Modeling"; }
    string icon() const override { return "icons/scale"; }
    int default_hotkey() const override { return 'R'; }
    bool has_gizmo() const override { return true; }
    GizmoMode gizmo_mode() const override { return GizmoMode::Scale; }
};

class OrbitTool : public ITool
{
public:
    string id() const override { return "orbit"; }
    string label() const override { return "Orbit"; }
    string description() const override { return "Orbit camera"; }
    string category() const override { return "Animation"; }
    string icon() const override { return "icons/orbit"; }
    int default_hotkey() const override { return 'A'; }
};

class PanTool : public ITool
{
public:
    string id() const override { return "pan"; }
    string label() const override { return "Pan"; }
    string description() const override { return "Pan camera"; }
    string category() const override { return "Animation"; }
    string icon() const override { return "icons/pan"; }
    int default_hotkey() const override { return 'S'; }
};

class ZoomTool : public ITool
{
public:
    string id() const override { return "zoom"; }
    string label() const override { return "Zoom"; }
    string description() const override { return "Zoom camera"; }
    string category() const override { return "Animation"; }
    string icon() const override { return "icons/zoom"; }
    int default_hotkey() const override { return 'D'; }
};

class SpawnTool : public ITool
{
public:
    string id() const override { return "spawn"; }
    string label() const override { return "Spawn"; }
    string description() const override { return "Spawn actor from template"; }
    string category() const override { return "Modeling"; }
    string icon() const override { return "icons/spawn"; }
    int default_hotkey() const override { return 'P'; }
};

class PlaybackTool : public ITool
{
public:
    string id() const override { return "playback"; }
    string label() const override { return "Playback"; }
    string description() const override { return "Control playback"; }
    string category() const override { return "Animation"; }
    string icon() const override { return "icons/play"; }
    int default_hotkey() const override { return ' '; }
};

class EraserTool : public ITool
{
public:
    string id() const override { return "eraser"; }
    string label() const override { return "Eraser"; }
    string description() const override { return "Remove actors"; }
    string category() const override { return "Modeling"; }
    string icon() const override { return "icons/eraser"; }
    int default_hotkey() const override { return 'X'; }
};

void register_builtin_tools();

} // namespace tools
} // namespace q3d