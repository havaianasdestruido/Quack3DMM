#include "material.h"

namespace q3d {
namespace render {

MaterialLibrary& MaterialLibrary::instance() { static MaterialLibrary lib; return lib; }

void MaterialLibrary::register_material(std::shared_ptr<MaterialDefinition> mat) {
    if (mat) materials_[mat->id] = mat;
}

std::shared_ptr<MaterialDefinition> MaterialLibrary::get(const char* id) const {
    auto it = materials_.find(id);
    return (it != materials_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<MaterialDefinition>> MaterialLibrary::all() const {
    std::vector<std::shared_ptr<MaterialDefinition>> result;
    for (const auto& [k, v] : materials_) result.push_back(v);
    return result;
}

std::vector<std::shared_ptr<MaterialDefinition>> MaterialLibrary::by_tag(const std::string&) const {
    return {};
}

bool MaterialLibrary::load_from_file(const char*) { return true; }
void MaterialLibrary::hot_reload_changed() {}

void MaterialLibrary::ensure_builtins() {
    if (materials_.empty()) {
        register_material(pbr());
        register_material(toon());
        register_material(unlit());
        register_material(fresnel());
        register_material(glass());
        register_material(skin());
        register_material(hair());
        register_material(outline());
        register_material(holographic());
    }
}

std::shared_ptr<MaterialDefinition> MaterialLibrary::pbr() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_pbr";
    mat->shader = "PBR";
    mat->queue = RenderQueue::Opaque;
    return mat;
}
std::shared_ptr<MaterialDefinition> MaterialLibrary::toon() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_toon";
    mat->shader = "Toon";
    mat->queue = RenderQueue::Opaque;
    return mat;
}
std::shared_ptr<MaterialDefinition> MaterialLibrary::unlit() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_unlit";
    mat->shader = "Unlit";
    mat->queue = RenderQueue::Opaque;
    return mat;
}
std::shared_ptr<MaterialDefinition> MaterialLibrary::fresnel() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_fresnel";
    mat->shader = "Fresnel";
    mat->queue = RenderQueue::Transparent;
    mat->blend_src = BlendMode::One;
    mat->blend_dst = BlendMode::One;
    return mat;
}
std::shared_ptr<MaterialDefinition> MaterialLibrary::glass() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_glass";
    mat->shader = "Glass";
    mat->queue = RenderQueue::Transparent;
    return mat;
}
std::shared_ptr<MaterialDefinition> MaterialLibrary::skin() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_skin";
    mat->shader = "Skin";
    mat->queue = RenderQueue::Opaque;
    return mat;
}
std::shared_ptr<MaterialDefinition> MaterialLibrary::hair() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_hair";
    mat->shader = "Hair";
    mat->queue = RenderQueue::Transparent;
    return mat;
}
std::shared_ptr<MaterialDefinition> MaterialLibrary::outline() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_outline";
    mat->shader = "Outline";
    mat->queue = RenderQueue::Opaque;
    mat->depth_write = false;
    mat->cull = CullMode::None;
    return mat;
}
std::shared_ptr<MaterialDefinition> MaterialLibrary::holographic() {
    auto mat = std::make_shared<MaterialDefinition>();
    mat->id = "builtin_holographic";
    mat->shader = "Holographic";
    mat->queue = RenderQueue::Transparent;
    return mat;
}

}
}
