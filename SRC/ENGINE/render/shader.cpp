#include "shader.h"
#include <algorithm>

namespace q3d {
namespace render {

static int fake_upload_uniform(uint32_t, const char*, int) { return 0; }

ShaderProgram::~ShaderProgram() {}

void ShaderProgram::bind() { if (!valid_) return; bound_ = true; }
void ShaderProgram::unbind() { bound_ = false; }

int ShaderProgram::get_uniform(const char* name) {
    auto it = uniform_cache_.find(name);
    if (it != uniform_cache_.end()) return it->second;
    int loc = fake_upload_uniform(program_id_, name, 0);
    uniform_cache_[name] = loc;
    return loc;
}

void ShaderProgram::set_int(const char* name, int v) { if (!bound_) return; (void)v; get_uniform(name); }
void ShaderProgram::set_float(const char* name, float v) { if (!bound_) return; (void)v; get_uniform(name); }
void ShaderProgram::set_vec2(const char* name, float x, float y) { if (!bound_) return; (void)x; (void)y; get_uniform(name); }
void ShaderProgram::set_vec3(const char* name, float x, float y, float z) { if (!bound_) return; (void)x; (void)y; (void)z; get_uniform(name); }
void ShaderProgram::set_vec4(const char* name, float x, float y, float z, float w) { if (!bound_) return; (void)x; (void)y; (void)z; (void)w; get_uniform(name); }
void ShaderProgram::set_mat4(const char* name, const Mat4& m) { if (!bound_) return; (void)m; get_uniform(name); }
void ShaderProgram::set_sampler(const char* name, int unit) { if (!bound_) return; (void)unit; get_uniform(name); }
void ShaderProgram::set_int_array(const char* name, const int* v, int count) { if (!bound_) return; (void)v; (void)count; get_uniform(name); }
void ShaderProgram::set_float_array(const char* name, const float* v, int count) { if (!bound_) return; (void)v; (void)count; get_uniform(name); }

}
}
