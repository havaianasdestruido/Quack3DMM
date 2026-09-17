#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "shader.h"

namespace q3d {
namespace render {

enum class RenderQueue { Opaque, Transparent };
enum class CompareFunc { Less, Always };
enum class BlendMode { One, Zero };
enum class CullMode { Back, None };

struct Variant {
    std::string str;
};

struct MaterialParam {
    enum class Type { Float, Int, Vec2, Vec3, Vec4, Mat4, Sampler };
    Type type;
    Variant value;
    int sampler_unit = 0;
};

struct MaterialDefinition {
    std::string id;
    std::string shader;
    std::unordered_map<std::string, MaterialParam> params;
    RenderQueue queue = RenderQueue::Opaque;
    bool depth_write = true;
    bool depth_test = true;
    CompareFunc depth_func = CompareFunc::Less;
    BlendMode blend_src = BlendMode::One;
    BlendMode blend_dst = BlendMode::Zero;
    CullMode cull = CullMode::Back;
    int render_pass = 0;
};

class MaterialLibrary {
public:
    static MaterialLibrary& instance();
    void register_material(std::shared_ptr<MaterialDefinition> mat);
    std::shared_ptr<MaterialDefinition> get(const char* id) const;
    std::vector<std::shared_ptr<MaterialDefinition>> all() const;
    std::vector<std::shared_ptr<MaterialDefinition>> by_tag(const std::string& tag) const;
    std::shared_ptr<MaterialDefinition> pbr();
    std::shared_ptr<MaterialDefinition> toon();
    std::shared_ptr<MaterialDefinition> unlit();
    std::shared_ptr<MaterialDefinition> fresnel();
    std::shared_ptr<MaterialDefinition> glass();
    std::shared_ptr<MaterialDefinition> skin();
    std::shared_ptr<MaterialDefinition> hair();
    std::shared_ptr<MaterialDefinition> outline();
    std::shared_ptr<MaterialDefinition> holographic();
    bool load_from_file(const char* vpath);
    void hot_reload_changed();
private:
    std::unordered_map<std::string, std::shared_ptr<MaterialDefinition>> materials_;
    void ensure_builtins();
};

}
}
