#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>

namespace q3d {
namespace import {

using byte = unsigned char;

struct Mat4 {
    float m[16];
    Mat4() { for (int i = 0; i < 16; ++i) m[i] = (i % 5 == 0) ? 1.0f : 0.0f; }
    float* data() { return m; }
    const float* data() const { return m; }
    float& operator()(int row, int col) { return m[col * 4 + row]; }
    const float& operator()(int row, int col) const { return m[col * 4 + row]; }
};

struct MorphTarget {
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tangents;
    float weight = 0.0f;
};

struct ImportedMaterial {
    std::string name;
    float baseColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float metallic = 0.0f;
    float roughness = 1.0f;
    float normalScale = 1.0f;
    float occlusionStrength = 1.0f;
    float emissive[3] = {0.0f, 0.0f, 0.0f};
    int baseColorTexture = -1;
    int metallicRoughnessTexture = -1;
    int normalTexture = -1;
    int occlusionTexture = -1;
    int emissiveTexture = -1;
    bool twoSided = false;
    enum AlphaMode { OPAQUE, MASK, BLEND } alphaMode = OPAQUE;
    float alphaCutoff = 0.5f;
};

struct ImportedMesh {
    std::string name;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> texcoords[10];
    std::vector<float> colors[10];
    std::vector<float> joint_weights[10];
    std::vector<int> joint_indices[10];
    std::vector<uint32_t> indices;
    std::vector<ImportedMaterial> materials;
    std::vector<std::string> joint_names;
    std::vector<Mat4> inverse_bind_matrices;
    std::vector<MorphTarget> morph_targets;
    int32_t primitive_mode = 4;
    size_t vertex_count = 0;
    size_t index_count = 0;
};

struct ImportedNode {
    std::string name;
    Mat4 local_transform;
    std::vector<int> children;
    int mesh_index = -1;
    int skin_index = -1;
    int camera_index = -1;
    int parent_index = -1;
};

struct AnimationChannel {
    enum Path { TRANSLATION, ROTATION, SCALE, WEIGHTS } path;
    int sampler_index = -1;
    int target_node = -1;
};

struct AnimationSampler {
    enum Interpolation { LINEAR, STEP, CUBICSPLINE } interpolation;
    std::vector<float> input_times;
    std::vector<float> output_values;
    int input_accessor = -1;
    int output_accessor = -1;
};

struct ImportedAnimation {
    std::string name;
    std::vector<AnimationChannel> channels;
    std::vector<AnimationSampler> samplers;
};

struct ImportedScene {
    std::string name;
    std::vector<int> nodes;
};

struct ImportedModel {
    std::string name;
    std::vector<ImportedMesh> meshes;
    std::vector<ImportedScene> scenes;
    std::vector<ImportedNode> nodes;
    std::vector<ImportedAnimation> animations;
    std::vector<std::string> extensions_used;
    int default_scene = 0;
};

struct AccessorData {
    std::vector<byte> raw_data;
    int count = 0;
    int component_type = 0;
    int type = 0;
    int buffer_view = -1;
    int byte_offset = 0;
    int stride = 0;
    bool normalized = false;
};

struct BufferViewData {
    int buffer = -1;
    size_t byte_offset = 0;
    size_t byte_length = 0;
    size_t byte_stride = 0;
    int target = 0;
};

struct BufferData {
    std::vector<byte> data;
    std::string uri;
};

using ProgressCallback = std::function<void(const char* stage, float progress)>;

class GltfImporter {
public:
    GltfImporter();
    ~GltfImporter();

    ImportedModel import(const char* vpath);
    ImportedModel import(const char* json_str, const std::vector<std::pair<const byte*, size_t>>& buffers);
    ImportedModel import_glb(const char* vpath);

    void set_dracoloader(void* decoder);
    void set_progress(ProgressCallback cb);

private:
    ImportedModel parse_json(const char* json_str);
    std::vector<byte> resolve_buffer(int buffer_index);
    AccessorData read_accessor(int acc_idx);
    std::vector<byte> read_buffer_view(const BufferViewData& bv);
    ImportedMesh parse_mesh(int mesh_idx);
    ImportedNode parse_node(int node_idx);
    ImportedAnimation parse_animation(int anim_idx);

    std::vector<std::pair<const byte*, size_t>> external_buffers_;
    std::vector<BufferData> buffers_;
    std::vector<BufferViewData> buffer_views_;
    std::vector<AccessorData> accessors_;
    void* draco_decoder_ = nullptr;
    ProgressCallback progress_cb_;
    float progress_ = 0.0f;

    void report_progress(const char* stage, float p);
    int parse_glb_header(const byte* data, size_t size, size_t* json_size, size_t* bin_size, size_t* bin_offset);
    std::vector<byte> base64_decode(const char* str, size_t len);
    Mat4 parse_node_transform(int node_idx);
};

} // namespace import
} // namespace q3d
