#include "render/shader_compiler.h"
#include <cassert>

using namespace q3d::render;

int main() {
    auto prog = ShaderCompiler::instance().compile("#version 330 core\nvoid main(){}", "#version 330 core\nout vec4 c; void main(){ c = vec4(1.0); }");
    assert(prog && prog->is_valid());
    prog->bind();
    prog->set_float("u_time", 1.0f);
    assert(prog->get_uniform("u_time") == prog->get_uniform("u_time"));
    ShaderCompiler::ValidationResult v = ShaderCompiler::instance().validate("uniform vec3 x;", ShaderStage::Vertex, ShaderLanguage::GLSL);
    assert(v.valid);
    return 0;
}