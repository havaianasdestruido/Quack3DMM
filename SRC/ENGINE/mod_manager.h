#pragma once

#include <string>
#include <vector>

#include "vfs.h"

namespace q3d {
namespace mod {

using StrPtr = const char*;

struct ModInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string author;
    std::string path;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflicts;
    int load_priority = 0;
    bool enabled = false;
    bool lazy = false;
};

class ModManager {
public:
    void scan();
    void resolve_load_order();
    void enable(StrPtr mod_id);
    void disable(StrPtr mod_id);
    const std::vector<ModInfo>& get_mods() const;
    void mount_all(q3d::vfs::VFS* vfs);

private:
    std::vector<ModInfo> _mods;
};

} // namespace mod
} // namespace q3d
