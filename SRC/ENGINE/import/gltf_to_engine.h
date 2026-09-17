#pragma once
#include "import/gltf_importer.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace q3d {
namespace import {

// Opaque engine type handles (defined by the host application).
struct BodyType;
struct ActorType;
struct AnimationType;
struct MaterialType;

enum class AnimImportMode {
    FullKeyframes,
    Compressed,
    Retarget
};

struct ConversionResult {
    std::shared_ptr<BodyType> body;
    std::shared_ptr<ActorType> actor;
    std::vector<std::shared_ptr<AnimationType>> animations;
    std::vector<std::shared_ptr<MaterialType>> materials;
};

class GltfToEngineConverter {
public:
    GltfToEngineConverter();

    ConversionResult convert(const ImportedModel& model, const char* template_id);

    void set_bone_alias(const char* from, const char* to);
    void set_skeleton_template(char* template_id);
    void set_material_override(const char* gltf_mat_name, char* engine_mat_id);
    void set_animation_import_mode(AnimImportMode mode);
    void set_animation_fps(float fps);

private:
    std::unordered_map<std::string, std::string> bone_aliases_;
    std::unordered_map<std::string, std::string> material_overrides_;
    std::string skeleton_template_id_;
    AnimImportMode anim_mode_ = AnimImportMode::FullKeyframes;
    float anim_fps_ = 30.0f;

    ConversionResult convert_meshes(const ImportedModel& model);
    void apply_materials(const ImportedModel& model, ConversionResult& out);
    void build_body_from_meshes(const ImportedModel& model, ConversionResult& out);
    void build_actor(const ImportedModel& model, ConversionResult& out);
    void build_animations(const ImportedModel& model, ConversionResult& out);
};

} // namespace import
} // namespace q3d