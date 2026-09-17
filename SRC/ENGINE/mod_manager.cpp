#include "mod_manager.h"

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace q3d {
namespace mod {

void ModManager::scan() {
    _mods.clear();
    fs::path root = fs::current_path() / "mods";
    std::error_code ec;
    if (!fs::exists(root, ec) || !fs::is_directory(root, ec))
        return;
    for (auto& entry : fs::directory_iterator(root, ec)) {
        if (!entry.is_directory())
            continue;
        ModInfo mi;
        mi.id = entry.path().filename().string();
        mi.name = mi.id;
        mi.version = "0.0.1";
        mi.author = "";
        mi.path = entry.path().string();
        mi.load_priority = 0;
        mi.enabled = true;
        mi.lazy = false;
        _mods.push_back(std::move(mi));
    }
}

void ModManager::resolve_load_order() {
    std::sort(_mods.begin(), _mods.end(), [](const ModInfo& a, const ModInfo& b) {
        return a.load_priority > b.load_priority;
    });
}

void ModManager::enable(StrPtr mod_id) {
    if (!mod_id)
        return;
    for (auto& m : _mods) {
        if (m.id == mod_id)
            m.enabled = true;
    }
}

void ModManager::disable(StrPtr mod_id) {
    if (!mod_id)
        return;
    for (auto& m : _mods) {
        if (m.id == mod_id)
            m.enabled = false;
    }
}

const std::vector<ModInfo>& ModManager::get_mods() const {
    return _mods;
}

void ModManager::mount_all(q3d::vfs::VFS* vfs) {
    if (!vfs)
        return;
    int base_priority = 10;
    for (const auto& m : _mods) {
        if (!m.enabled)
            continue;
        vfs->mount_layer(m.path.c_str(), base_priority, false);
        base_priority++;
    }
}

} // namespace mod
} // namespace q3d
