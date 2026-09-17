#pragma once

#include <memory>
#include <string>
#include <vector>

namespace q3d {
namespace vfs {

using StrPtr = const char*;

struct AssetEntry {
    std::string vpath;
    std::string physical_path;
    bool is_directory = false;
};

using OnChange = void(*)(const std::string& vpath);

class IStream {
public:
    virtual ~IStream() = default;
    virtual std::size_t read(void* buffer, std::size_t bytes) = 0;
};

class OStream {
public:
    virtual ~OStream() = default;
    virtual std::size_t write(const void* buffer, std::size_t bytes) = 0;
};

class VFS {
public:
    void mount_layer(StrPtr path, int priority, bool is_archive = false);
    std::shared_ptr<AssetEntry> resolve(const char* vpath);
    std::unique_ptr<IStream> open_read(const char* vpath);
    std::unique_ptr<OStream> open_write(const char* vpath);
    bool exists(const char* vpath);
    void list(const char* vpath, std::vector<AssetEntry>& out);
    void add_watcher(OnChange callback);

private:
    struct Layer {
        std::string root_path;
        int priority;
        bool is_archive;
    };

    std::vector<Layer> _layers;
    std::vector<OnChange> _watchers;
};

} // namespace vfs
} // namespace q3d
