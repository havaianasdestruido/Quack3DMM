#pragma once

#include <memory>
#include <string>

#include "vfs.h"

namespace q3m {

class Package {
public:
    static std::unique_ptr<Package> create(q3d::vfs::StrPtr path);

    bool contains(const char* vpath_in_package) const;
    std::unique_ptr<q3d::vfs::IStream> open(const char* vpath_in_package);

    const std::string& manifest_path() const;

private:
    std::string _path;
    std::string _manifest_path;
};

enum class Compression {
    Deflate,
    Zstd,
};

class PackageWriter {
public:
    static void pack(const char* input_dir, const char* output_q3m, Compression compr = Compression::Zstd);
    static void unpack(const char* q3m_path, const char* output_dir);
    static void inspect(const char* q3m_path);
};

} // namespace q3m
