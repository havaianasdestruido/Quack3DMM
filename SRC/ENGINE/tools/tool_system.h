#pragma once

#include <string>
#include <vector>
#include <memory>

namespace q3d {
namespace tools {

using std::string;
using std::vector;
using std::shared_ptr;

// Opaque engine type handles (forward-declared, defined by host app)
struct MvMovie;
struct MvScene;
struct MvActor;

using MovieHandle = MvMovie*;
using SceneHandle = MvScene*;
using ActorHandle = MvActor*;

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct Ray {
    Vec3 origin;
    Vec3 direction;
};

struct HitResult {
    bool hit = false;
    float distance = 0.0f;
    Vec3 point;
    Vec3 normal;
    ActorHandle actor = nullptr;
};

class ToolContext;
class Renderer;
class Viewport;

enum class ToolCategory {
    Select,
    Modeling,
    Animation,
    Painting,
    Scripting,
    Debug,
    Custom,
};

enum class ToolCursor {
    Arrow,
    Crosshair,
    Move,
    Rotate,
    Scale,
    Brush,
    Eyedropper,
    Grab,
    Wait,
    Pen,
    Custom,
};

enum class GizmoMode {
    Translate,
    Rotate,
    Scale,
    ScaleNonUniform,
};

class ITool {
public:
    virtual ~ITool() = default;

    virtual string id() const = 0;
    virtual string label() const = 0;
    virtual string description() const = 0;
    virtual string category() const = 0;
    virtual string icon() const = 0;
    virtual int default_hotkey() const = 0;
    virtual int hotkey_mods() const { return 0; }

    virtual void activate(ToolContext* ctx);
    virtual void deactivate(ToolContext* ctx);

    virtual void on_frame(ToolContext* ctx, float dt);

    virtual bool on_mouse_move(ToolContext* ctx, int x, int y, int buttons);
    virtual bool on_mouse_down(ToolContext* ctx, int x, int y, int button);
    virtual bool on_mouse_up(ToolContext* ctx, int x, int y, int button);
    virtual bool on_key_down(ToolContext* ctx, int key, int mods);
    virtual bool on_key_up(ToolContext* ctx, int key, int mods);
    virtual bool on_char(ToolContext* ctx, int ch);
    virtual bool on_scroll(ToolContext* ctx, int dx, int dy);

    virtual bool on_click(ToolContext* ctx, const Ray& ray, const HitResult& hit);
    virtual bool on_drag_start(ToolContext* ctx, const Ray& ray, const HitResult& hit);
    virtual bool on_drag_move(ToolContext* ctx, const Ray& ray, const Vec3& delta);
    virtual bool on_drag_end(ToolContext* ctx);

    virtual bool has_gizmo() const { return false; }
    virtual GizmoMode gizmo_mode() const { return GizmoMode::Translate; }
    virtual void apply_gizmo(ToolContext* ctx, const Vec3& delta, GizmoMode mode);

    virtual void render_overlay(ToolContext* ctx, Renderer* renderer);

    virtual ToolCursor cursor() const { return ToolCursor::Arrow; }

    virtual bool is_modal() const { return false; }

    virtual string state_id() const { return ""; }
    virtual bool can_undo() const { return false; }
    virtual void undo() {}
    virtual bool can_redo() const { return false; }
    virtual void redo() {}
};

class ToolContext {
public:
    MovieHandle movie;
    SceneHandle scene;
    Viewport* viewport;
    vector<ActorHandle> selected_actors;
    ActorHandle hovered_actor;
    Vec3 hover_point;
    Vec3 gizmo_position;
    void* user_data;

    ToolContext(MovieHandle m, SceneHandle s, Viewport* v);

    ActorHandle spawn_actor(const char* template_id);
    void delete_selection();
    void select(ActorHandle a);
    void deselect(ActorHandle a);
    void select_all();
    void deselect_all();
    void undo();
    void redo();
    Ray screen_to_ray(int x, int y) const;
    HitResult raycast(const Ray& ray) const;
    void mark_dirty();
};

} // namespace tools
} // namespace q3d