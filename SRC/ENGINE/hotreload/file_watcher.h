#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>

namespace q3d {
namespace hotreload {

struct FileChange;

enum class FileChangeType;

class FileWatcher {
public:
    using Callback = std::function<void(const std::vector<FileChange>& changes)>;

    FileWatcher();
    ~FileWatcher();

    void watch(const char* path);
    void watch(const std::vector<std::string>& paths);

    void unwatch(const char* path);
    void unwatch_all();

    void set_callback(Callback cb);

    void poll();

    std::vector<FileChange> flush();

    void set_debounce_ms(int ms) { debounce_ms_ = ms; }

private:
    struct Impl;
    std::unique_ptr<Impl> p_;
    int debounce_ms_ = 100;
    std::vector<FileChange> pending_;
    std::mutex pending_mutex_;
};

struct FileChange {
    std::string path;
    std::string vpath;
    FileChangeType type;
    long mtime;
    long size;
    std::string old_path;
    std::string mod_id;
};

enum class FileChangeType {
    Created,
    Modified,
    Deleted,
    Renamed,
};

} // namespace hotreload
} // namespace q3d
