#pragma once

#include "mod_manifest.h"
#include "vfs.h"

namespace q3d {
namespace mod {

using StrPtr = const char*;

class ModEntry {
public:
    ModEntry(ModManifest manifest, q3d::vfs::VFS* vfs);

    const ModManifest& manifest() const { return manifest_; }
    const std::string& id() const { return manifest_.id; }
    TrustTier trust_tier() const { return manifest_.trust_tier; }

    bool has_asset(StrPtr vpath) const;
    std::unique_ptr<q3d::vfs::IStream> open_asset(StrPtr vpath);

private:
    ModManifest manifest_;
    q3d::vfs::VFS* vfs_ = nullptr;
};

} // namespace mod
} // namespace q3d