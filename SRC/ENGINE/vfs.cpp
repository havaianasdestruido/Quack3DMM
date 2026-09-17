#include "vfs.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace q3d {
namespace vfs {

namespace fs = std::filesystem;

class FileIStream : public IStream {
public:
    explicit FileIStream(const fs::path& p) : _ifs(p, std::ios::binary) {}
    std::size_t read(void* buffer, std::size_t bytes) override {
        if (!_ifs)
            return 0;
        _ifs.read(static_cast<char*>(buffer), static_cast<std::streamsize>(bytes));
        return static_cast<std::size_t>(_ifs.gcount());
    }
private:
    std::ifstream _ifs;
};

class FileOStream : public OStream {
public:
    explicit FileOStream(const fs::path& p) : _ofs(p, std::ios::binary) {}
    std::size_t write(const void* buffer, std::size_t bytes) override {
        if (!_ofs)
            return 0;
        _ofs.write(static_cast<const char*>(buffer), static_cast<std::streamsize>(bytes));
        return bytes;
    }
private:
    std::ofstream _ofs;
};

void VFS::mount_layer(StrPtr path, int priority, bool is_archive) {
    if (!path)
        return;
    Layer layer{std::string(path), priority, is_archive};
    _layers.push_back(layer);
    std::sort(_layers.begin(), _layers.end(), [](const Layer& a, const Layer& b) {
        return a.priority > b.priority;
    });
}

std::shared_ptr<AssetEntry> VFS::resolve(const char* vpath) {
    if (!vpath)
        return nullptr;
    for (const auto& layer : _layers) {
        if (layer.is_archive)
            continue;
        fs::path physical = fs::path(layer.root_path) / vpath;
        std::error_code ec;
        if (fs::exists(physical, ec)) {
            auto entry = std::make_shared<AssetEntry>();
            entry->vpath = vpath;
            entry->physical_path = physical.string();
            entry->is_directory = fs::is_directory(physical, ec);
            return entry;
        }
    }
    return nullptr;
}

std::unique_ptr<IStream> VFS::open_read(const char* vpath) {
    auto entry = resolve(vpath);
    if (!entry || entry->is_directory)
        return nullptr;
    return std::make_unique<FileIStream>(fs::path(entry->physical_path));
}

std::unique_ptr<OStream> VFS::open_write(const char* vpath) {
    if (_layers.empty())
        return nullptr;
    const auto& write_layer = _layers.front();
    fs::path physical = fs::path(write_layer.root_path) / vpath;
    fs::create_directories(physical.parent_path());
    return std::make_unique<FileOStream>(physical);
}

bool VFS::exists(const char* vpath) {
    return static_cast<bool>(resolve(vpath));
}

void VFS::list(const char* vpath, std::vector<AssetEntry>& out) {
    out.clear();
    for (const auto& layer : _layers) {
        if (layer.is_archive)
            continue;
        fs::path base = fs::path(layer.root_path) / vpath;
        std::error_code ec;
        if (!fs::exists(base, ec) || !fs::is_directory(base, ec))
            continue;
        for (auto& dir_entry : fs::directory_iterator(base, ec)) {
            AssetEntry e;
            e.vpath = vpath;
            e.physical_path = dir_entry.path().string();
            e.is_directory = dir_entry.is_directory();
            out.push_back(std::move(e));
        }
    }
}

void VFS::add_watcher(OnChange callback) {
    if (callback)
        _watchers.push_back(callback);
}

} // namespace vfs
} // namespace q3d
