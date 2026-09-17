#pragma once
#include "shader.h"
#include <unordered_set>

namespace q3d {
namespace render {

class ShaderCompiler {
public:
    static ShaderCompiler& instance();
    std::shared_ptr<ShaderProgram> compile(const char* vertex_src,
                                           const char* fragment_src,
                                           const char* compute_src = nullptr,
                                           const char* geometry_src = nullptr,
                                           ShaderLanguage lang = ShaderLanguage::GLSL);
    std::shared_ptr<ShaderProgram> compile_spirv(const std::vector<uint32_t>& vertex_spirv,
                                                 const std::vector<uint32_t>& fragment_spirv);
    std::shared_ptr<ShaderProgram> compile_file(const char* vpath);
    std::shared_ptr<ShaderProgram> compile_dsl(const char* dsl_source, const char* name = "=");
    void cross_compile(const char* glsl_src,
                       ShaderStage stage,
                       ShaderLanguage target,
                       string& output,
                       string& errors);
    void add_include_path(const char* vpath);
    void remove_include_path(const char* vpath);
    struct ValidationResult {
        bool valid = false;
        std::vector<string> errors;
        std::vector<string> warnings;
        std::vector<string> uniforms_used;
        std::vector<string> samplers_used;
        int estimated_instruction_count = 0;
    };
    ValidationResult validate(const char* src, ShaderStage stage, ShaderLanguage lang);
    void set_hot_reload_callback(std::function<void(ShaderProgram*)> cb);
    void set_backend(ShaderBackend backend);
    ShaderBackend backend() const { return backend_; }
private:
    ShaderBackend backend_ = ShaderBackend::OpenGL;
    std::unordered_set<string> include_paths_;
    std::function<void(ShaderProgram*)> hot_reload_cb_;
};

}
}
