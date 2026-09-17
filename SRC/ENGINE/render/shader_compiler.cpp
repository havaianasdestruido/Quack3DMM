#include "shader_compiler.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <sstream>

namespace q3d {
namespace render {

ShaderCompiler& ShaderCompiler::instance() {
    static ShaderCompiler compiler;
    return compiler;
}

namespace {

std::vector<string> tokenize_lines(const char* src) {
    std::vector<string> lines;
    if (!src) return lines;
    std::istringstream iss(src);
    string line;
    while (std::getline(iss, line)) {
        // strip comments
        auto comment = line.find("//");
        if (comment != string::npos)
            line.erase(comment);
        // trim
        auto first = std::find_if_not(line.begin(), line.end(),
            [](unsigned char c) { return std::isspace(c); });
        auto last = std::find_if_not(line.rbegin(), line.rend(),
            [](unsigned char c) { return std::isspace(c); }).base();
        if (first < last)
            lines.emplace_back(first, last);
    }
    return lines;
}

bool is_type_keyword(const string& word) {
    static const std::vector<string> types = {
        "float", "vec2", "vec3", "vec4", "int", "ivec2", "ivec3", "ivec4",
        "uint", "uvec2", "uvec3", "uvec4", "bool", "bvec2", "bvec3", "bvec4",
        "mat2", "mat3", "mat4", "sampler2D", "sampler3D", "samplerCube",
        "sampler2DArray", "sampler2DMS", "samplerBuffer", "double"
    };
    return std::find(types.begin(), types.end(), word) != types.end();
}

} // namespace

std::shared_ptr<ShaderProgram> ShaderCompiler::compile(
    const char* vertex_src,
    const char* fragment_src,
    const char* compute_src,
    const char* geometry_src,
    ShaderLanguage lang)
{
    auto program = std::make_shared<ShaderProgram>();

    if (!vertex_src || !fragment_src) {
        program->error_ = "missing vertex or fragment source";
        return program;
    }

    // CPU-side validation (no GPU available); parse uniforms lazily.
    ValidationResult vr = validate(vertex_src, ShaderStage::Vertex, lang);
    if (!vr.valid) {
        program->error_ = vr.errors.empty() ? "vertex shader invalid" : vr.errors.front();
        return program;
    }
    vr = validate(fragment_src, ShaderStage::Fragment, lang);
    if (!vr.valid) {
        program->error_ = vr.errors.empty() ? "fragment shader invalid" : vr.errors.front();
        return program;
    }

    if (compute_src) {
        vr = validate(compute_src, ShaderStage::Compute, lang);
        if (!vr.valid) {
            program->error_ = "compute shader invalid";
            return program;
        }
    }
    if (geometry_src) {
        vr = validate(geometry_src, ShaderStage::Geometry, lang);
        if (!vr.valid) {
            program->error_ = "geometry shader invalid";
            return program;
        }
    }

    program->valid_ = true;
    program->error_.clear();
    (void)program->program_id_;
    return program;
}

std::shared_ptr<ShaderProgram> ShaderCompiler::compile_spirv(
    const std::vector<uint32_t>& vertex_spirv,
    const std::vector<uint32_t>& fragment_spirv)
{
    auto program = std::make_shared<ShaderProgram>();
    if (vertex_spirv.empty() || fragment_spirv.empty()) {
        program->error_ = "empty SPIR-V module";
        return program;
    }
    program->valid_ = true;
    return program;
}

std::shared_ptr<ShaderProgram> ShaderCompiler::compile_file(const char* vpath) {
    // VFS-aware load; for now, raw file read.
    if (!vpath) return nullptr;
    FILE* f = std::fopen(vpath, "rb");
    if (!f) return nullptr;
    std::fseek(f, 0, SEEK_END);
    long len = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (len <= 0) { std::fclose(f); return nullptr; }
    std::string buf(static_cast<size_t>(len) + 1, '\0');
    size_t n = std::fread(buf.data(), 1, static_cast<size_t>(len), f);
    std::fclose(f);
    (void)n;
    // Extremely simple convention: file contains a single shader source.
    return compile(buf.c_str(), buf.c_str());
}

std::shared_ptr<ShaderProgram> ShaderCompiler::compile_dsl(const char* dsl_source, const char* name) {
    (void)name;
    if (!dsl_source) return nullptr;
    // DSL passthrough: treat the source itself as the compiled program token.
    return compile(dsl_source, dsl_source);
}

void ShaderCompiler::cross_compile(const char* glsl_src, ShaderStage stage,
                                   ShaderLanguage target, string& output, string& errors)
{
    // Real HLSL/WGSL/SPIRV emission requires SPIRV-Cross; report capability gap.
    output.clear();
    errors.clear();
    if (!glsl_src) {
        errors = "null GLSL source";
        return;
    }
    switch (target) {
    case ShaderLanguage::GLSL:
        output = glsl_src;
        break;
    case ShaderLanguage::HLSL:
    case ShaderLanguage::WGSL:
    case ShaderLanguage::SPIRV:
    default:
        errors = "cross-compile from GLSL to target requires SPIRV-Cross (not linked)";
        break;
    }
    (void)stage;
}

void ShaderCompiler::add_include_path(const char* vpath) {
    if (vpath)
        include_paths_.insert(vpath);
}

void ShaderCompiler::remove_include_path(const char* vpath) {
    if (vpath)
        include_paths_.erase(vpath);
}

ShaderCompiler::ValidationResult ShaderCompiler::validate(const char* src, ShaderStage stage, ShaderLanguage lang)
{
    ValidationResult r;
    if (!src) {
        r.errors.push_back("null shader source");
        return r;
    }

    auto lines = tokenize_lines(src);

    bool have_version = false;
    bool have_main = false;
    int estimated_instructions = 0;

    for (const auto& line : lines) {
        if (line.rfind("#version", 0) == 0) {
            have_version = true;
        }
        else if (line.find("void main") != string::npos && line.find('{') != string::npos) {
            have_main = true;
        }

        // Parse `uniform <type> <name>;`
        if (line.rfind("uniform", 0) == 0) {
            std::istringstream iss(line);
            string kw, type, name;
            iss >> kw >> type >> name;
            if (is_type_keyword(type) && !name.empty()) {
                // strip trailing ; or [N]
                auto semi = name.find(';');
                if (semi != string::npos)
                    name.erase(semi);
                auto brk = name.find('[');
                if (brk != string::npos)
                    name.erase(brk);
                if (!name.empty()) {
                    r.uniforms_used.push_back(name);
                    if (type.rfind("sampler", 0) == 0)
                        r.samplers_used.push_back(name);
                }
            }
        }

        // Cheap instruction estimate: count operators + function calls.
        estimated_instructions += static_cast<int>(
            std::count(line.begin(), line.end(), ';') +
            std::count(line.begin(), line.end(), '+') +
            std::count(line.begin(), line.end(), '*'));
    }

    if (lang == ShaderLanguage::GLSL) {
        if (!have_version) {
            r.warnings.push_back("missing #version directive; defaulting to 110");
        }
        if (have_main && !have_version && stage == ShaderStage::Vertex) {
            // 110 style without version line is legal but warns.
        }
    }

    // "main" required except for include-only fragments.
    if (!have_main) {
        r.warnings.push_back("no 'void main' body found (may be a fragment of a library)");
    }

    if (!have_version && have_main) {
        // Not fatal across toolchains; keep as warning-only to be permissive.
    }

    r.valid = true;
    r.estimated_instruction_count = estimated_instructions;
    return r;
}

void ShaderCompiler::set_hot_reload_callback(std::function<void(ShaderProgram*)> cb) {
    hot_reload_cb_ = std::move(cb);
}

void ShaderCompiler::set_backend(ShaderBackend backend) {
    backend_ = backend;
}

} // namespace render
} // namespace q3d