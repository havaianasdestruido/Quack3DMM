#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace q3d {
namespace render {

struct Mat4 { float m[16]{}; };
using string = std::string;
using StrPtr = const char*;

enum class ShaderLanguage { GLSL, HLSL, WGSL, SPIRV };
enum class ShaderStage { Vertex, Fragment, Compute, Geometry, TessControl, TessEval };
enum class ShaderBackend { OpenGL, Vulkan, D3D11, D3D12, Metal, WebGPU };

class ShaderProgram {
public:
    ~ShaderProgram();
    bool is_valid() const { return valid_; }
    const string& error() const { return error_; }
    void bind();
    void unbind();
    bool is_bound() const { return bound_; }
    void set_int(const char* name, int v);
    void set_float(const char* name, float v);
    void set_vec2(const char* name, float x, float y);
    void set_vec3(const char* name, float x, float y, float z);
    void set_vec4(const char* name, float x, float y, float z, float w);
    void set_mat4(const char* name, const Mat4& m);
    void set_sampler(const char* name, int unit);
    void set_int_array(const char* name, const int* v, int count);
    void set_float_array(const char* name, const float* v, int count);
    int get_uniform(const char* name);
private:
    friend class ShaderCompiler;
    friend class ShaderManager;
    uint32_t program_id_ = 0;
    std::unordered_map<string, int> uniform_cache_;
    bool valid_ = false;
    string error_;
    ShaderStage stage_ = ShaderStage::Vertex;
    bool bound_ = false;
};

}
}
