#include "asset_reloader.h"

#include <cstring>

namespace q3d {
namespace hotreload {

void TextureReloader::reload(StrPtr vpath) {
    // Host texture manager hooks in here; CPU-side no-op.
    (void)vpath;
}

void TextureReloader::gpu_reload(int texture_id, const std::string& vpath) {
    (void)texture_id;
    (void)vpath;
}

void ModelReloader::reload(StrPtr vpath) {
    (void)vpath;
}

void ModelReloader::gpu_reload(int mesh_id, const std::string& vpath) {
    (void)mesh_id;
    (void)vpath;
}

void ShaderReloader::reload(StrPtr vpath) {
    (void)vpath;
}

void ShaderReloader::gpu_reload(int program_id, const std::string& vpath) {
    (void)program_id;
    (void)vpath;
}

void ScriptReloader::reload(StrPtr vpath) {
    (void)vpath;
}

void ScriptReloader::gpu_reload(void* runtime, const std::string& vpath) {
    (void)runtime;
    (void)vpath;
}

void UIReloader::reload(StrPtr vpath) {
    (void)vpath;
}

void AudioReloader::reload(StrPtr vpath) {
    (void)vpath;
}

void AssetReloader::reload(StrPtr vpath, ReloadType type) {
    if (!vpath) return;
    switch (type) {
    case ReloadType::Texture:        TextureReloader{}.reload(vpath); break;
    case ReloadType::Model:          ModelReloader{}.reload(vpath); break;
    case ReloadType::Shader:         ShaderReloader{}.reload(vpath); break;
    case ReloadType::ScriptLua:
    case ReloadType::ScriptPython:
    case ReloadType::ScriptWasm:     ScriptReloader{}.reload(vpath); break;
    case ReloadType::UIPanel:        UIReloader{}.reload(vpath); break;
    case ReloadType::Sound:          AudioReloader{}.reload(vpath); break;
    default: break;
    }
}

const char* AssetReloader::extension_for_type(ReloadType type) {
    switch (type) {
    case ReloadType::Texture:     return ".png";
    case ReloadType::Model:       return ".gltf";
    case ReloadType::Material:    return ".mat";
    case ReloadType::Shader:      return ".glsl";
    case ReloadType::ScriptLua:   return ".lua";
    case ReloadType::ScriptPython:return ".py";
    case ReloadType::ScriptWasm:  return ".wasm";
    case ReloadType::UIPanel:     return ".ui";
    case ReloadType::Scene:       return ".3mm";
    case ReloadType::Sound:       return ".wav";
    case ReloadType::Font:        return ".ttf";
    case ReloadType::Config:      return ".toml";
    default: return "";
    }
}

ReloadType AssetReloader::type_for_extension(const char* ext) {
    if (!ext) return ReloadType::Config;
    if (std::strcmp(ext, ".png") == 0 || std::strcmp(ext, ".jpg") == 0 || std::strcmp(ext, ".bmp") == 0)
        return ReloadType::Texture;
    if (std::strcmp(ext, ".obj") == 0 || std::strcmp(ext, ".fbx") == 0 || std::strcmp(ext, ".gltf") == 0)
        return ReloadType::Model;
    if (std::strcmp(ext, ".glsl") == 0 || std::strcmp(ext, ".hlsl") == 0 || std::strcmp(ext, ".wgsl") == 0)
        return ReloadType::Shader;
    if (std::strcmp(ext, ".lua") == 0)
        return ReloadType::ScriptLua;
    if (std::strcmp(ext, ".py") == 0)
        return ReloadType::ScriptPython;
    if (std::strcmp(ext, ".wasm") == 0)
        return ReloadType::ScriptWasm;
    if (std::strcmp(ext, ".wav") == 0 || std::strcmp(ext, ".mp3") == 0 || std::strcmp(ext, ".ogg") == 0)
        return ReloadType::Sound;
    if (std::strcmp(ext, ".3mm") == 0)
        return ReloadType::Scene;
    if (std::strcmp(ext, ".toml") == 0 || std::strcmp(ext, ".json") == 0 || std::strcmp(ext, ".cfg") == 0)
        return ReloadType::Config;
    if (std::strcmp(ext, ".ttf") == 0 || std::strcmp(ext, ".otf") == 0)
        return ReloadType::Font;
    return ReloadType::Config;
}

} // namespace hotreload
} // namespace q3d