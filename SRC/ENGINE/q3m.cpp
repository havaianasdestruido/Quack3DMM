#include "q3m.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace q3m {

std::unique_ptr<Package> Package::create(q3d::vfs::StrPtr path) {
    if (!path)
        return nullptr;
    fs::path p(path);
    if (!fs::exists(p))
        return nullptr;
    auto pkg = std::unique_ptr<Package>(new Package());
    pkg->_path = p.string();
    pkg->_manifest_path = "mod.toml";
    return pkg;
}

bool Package::contains(const char* vpath_in_package) const {
    // ponytail: real ZIP index later, for now only manifest.
    if (!vpath_in_package)
        return false;
    return std::string(vpath_in_package) == _manifest_path;
}

std::unique_ptr<q3d::vfs::IStream> Package::open(const char* vpath_in_package) {
    // ponytail: streaming from ZIP later.
    if (!contains(vpath_in_package))
        return nullptr;
    return nullptr;
}

const std::string& Package::manifest_path() const {
    return _manifest_path;
}

void PackageWriter::pack(const char* input_dir, const char* output_q3m, Compression) {
    // ponytail: implement real ZIP writer.
    fs::copy(input_dir, output_q3m, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
}

void PackageWriter::unpack(const char* q3m_path, const char* output_dir) {
    // ponytail: implement real ZIP reader.
    fs::copy(q3m_path, output_dir, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
}

void PackageWriter::inspect(const char* q3m_path) {
    // ponytail: implement real ZIP listing.
    fs::path p(q3m_path);
    if (!fs::exists(p))
        return;
}

} // namespace q3m
