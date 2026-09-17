#include "mod.h"

namespace q3d {
namespace mod {

ModEntry::ModEntry(ModManifest manifest, q3d::vfs::VFS* vfs)
    : manifest_(std::move(manifest)), vfs_(vfs) {}

bool ModEntry::has_asset(StrPtr vpath) const {
    if (!vpath || !vfs_)
        return false;
    return vfs_->exists(vpath);
}

std::unique_ptr<q3d::vfs::IStream> ModEntry::open_asset(StrPtr vpath) {
    if (!vpath || !vfs_)
        return nullptr;
    return vfs_->open_read(vpath);
}

} // namespace mod
} // namespace q3d