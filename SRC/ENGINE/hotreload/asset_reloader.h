#pragma once

#include <string>
#include "reload_manager.h"

namespace q3d {
namespace hotreload {

class TextureReloader { public: void reload(StrPtr vpath); void gpu_reload(int, const std::string&); };
class ModelReloader { public: void reload(StrPtr vpath); void gpu_reload(int, const std::string&); };
class ShaderReloader { public: void reload(StrPtr vpath); void gpu_reload(int, const std::string&); };
class ScriptReloader { public: void reload(StrPtr vpath); void gpu_reload(void*, const std::string&); };
class UIReloader { public: void reload(StrPtr vpath); };
class AudioReloader { public: void reload(StrPtr vpath); };

class AssetReloader {
public:
    static void reload(StrPtr vpath, ReloadType type);
    static const char* extension_for_type(ReloadType type);
    static ReloadType type_for_extension(const char* ext);
};

} // namespace hotreload
} // namespace q3d
