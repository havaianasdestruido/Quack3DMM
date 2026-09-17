#include "import/gltf_to_engine.h"

namespace q3d {
namespace import {

GltfToEngineConverter::GltfToEngineConverter() {}

void GltfToEngineConverter::set_bone_alias(const char* from, const char* to)
{
    if (from && to)
        bone_aliases_[from] = to;
}

void GltfToEngineConverter::set_skeleton_template(char* template_id)
{
    skeleton_template_id_ = template_id ? template_id : "";
}

void GltfToEngineConverter::set_material_override(const char* gltf_mat_name, char* engine_mat_id)
{
    if (gltf_mat_name && engine_mat_id)
        material_overrides_[gltf_mat_name] = engine_mat_id;
}

void GltfToEngineConverter::set_animation_import_mode(AnimImportMode mode)
{
    anim_mode_ = mode;
}

void GltfToEngineConverter::set_animation_fps(float fps)
{
    anim_fps_ = fps > 0.0f ? fps : 30.0f;
}

ConversionResult GltfToEngineConverter::convert(const ImportedModel& model, const char* template_id)
{
    ConversionResult out;
    build_body_from_meshes(model, out);
    build_actor(model, out);
    apply_materials(model, out);
    build_animations(model, out);
    return out;
}

ConversionResult GltfToEngineConverter::convert_meshes(const ImportedModel& model)
{
    return ConversionResult();
}

void GltfToEngineConverter::apply_materials(const ImportedModel& model, ConversionResult& out)
{
}

void GltfToEngineConverter::build_body_from_meshes(const ImportedModel& model, ConversionResult& out)
{
}

void GltfToEngineConverter::build_actor(const ImportedModel& model, ConversionResult& out)
{
}

void GltfToEngineConverter::build_animations(const ImportedModel& model, ConversionResult& out)
{
}

} // namespace import
} // namespace q3d
