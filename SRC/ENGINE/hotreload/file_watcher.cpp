#include "file_watcher.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fileapi.h>
#include <debugapi.h>
#ifndef FILE_ACTION_ADDED
#define FILE_ACTION_ADDED 0x00000001
#endif
#ifndef FILE_ACTION_REMOVED
#define FILE_ACTION_REMOVED 0x00000002
#endif
#ifndef FILE_ACTION_MODIFIED
#define FILE_ACTION_MODIFIED 0x00000003
#endif
#ifndef FILE_ACTION_RENAMED_OLD_NAME
#define FILE_ACTION_RENAMED_OLD_NAME 0x00000004
#endif
#ifndef FILE_ACTION_RENAMED_NEW_NAME
#define FILE_ACTION_RENAMED_NEW_NAME 0x00000005
#endif
#elif defined(__APPLE__)
#include <CoreServices/CoreServices.h>
#else
#include <sys/inotify.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <chrono>
#include <cstring>
#include <map>
#include <thread>
#include <algorithm>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

namespace q3d {
namespace hotreload {

namespace {

constexpr size_t kBufSize = 65536;

#if defined(_WIN32)
DWORD notify_filters = FILE_NOTIFY_CHANGE_FILE_NAME
                    | FILE_NOTIFY_CHANGE_DIR_NAME
                    | FILE_NOTIFY_CHANGE_LAST_WRITE
                    | FILE_NOTIFY_CHANGE_SIZE;

#elif defined(__APPLE__)
FSEventStreamCreateFlags kFSEventFlags =
    kFSEventStreamCreateFlagFileEvents
    | kFSEventStreamCreateFlagUseCFTypes;
#endif

long get_file_mtime(const char* path) {
#if defined(_WIN32)
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    if (GetFileAttributesExA(path, GetFileExInfoStandard, &attrs)) {
        ULARGE_INTEGER ull;
        ull.LowPart = attrs.ftLastWriteTime.dwLowDateTime;
        ull.HighPart = attrs.ftLastWriteTime.dwHighDateTime;
        return static_cast<long>(ull.QuadPart / 10000000 - 11644473600LL);
    }
#elif defined(__APPLE__)
    struct stat st;
    if (stat(path, &st) == 0) {
        return static_cast<long>(st.st_mtime);
    }
#else
    struct stat st;
    if (stat(path, &st) == 0) {
        return static_cast<long>(st.st_mtime);
    }
#endif
    return 0;
}

long get_file_size(const char* path) {
#if defined(_WIN32)
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    if (GetFileAttributesExA(path, GetFileExInfoStandard, &attrs)) {
        return (static_cast<long>(attrs.nFileSizeHigh) << 32) | attrs.nFileSizeLow;
    }
#else
    struct stat st;
    if (stat(path, &st) == 0) {
        return static_cast<long>(st.st_size);
    }
#endif
    return 0;
}

}

struct FileWatcher::Impl {
    std::vector<std::string> watch_paths;
    Callback callback;
    std::vector<FileChange> pending;
    std::mutex pending_mutex;
    int debounce_ms = 100;
    std::map<std::string, long> path_mtimes;
    std::chrono::steady_clock::time_point last_debounce = std::chrono::steady_clock::now();

#if defined(_WIN32)
    struct WinWatch {
        HANDLE dir_handle = INVALID_HANDLE_VALUE;
        HANDLE stop_event = INVALID_HANDLE_VALUE;
        std::thread thread;
        std::vector<BYTE> buffer;
        OVERLAPPED ol = {};
    };
    std::map<std::string, WinWatch> watches;
#elif defined(__APPLE__)
    FSEventStreamRef stream = nullptr;
    std::thread thread;
    FSEventStreamStop(nullptr);
#elif defined(__linux__)
    int inotify_fd = -1;
    std::thread thread;
    std::map<int, std::string> wd_to_path;
#endif

    ~Impl() { stop_all(); }

    void stop_all() {
#if defined(_WIN32)
        for (auto& w : watches) {
            if (w.second.stop_event != INVALID_HANDLE_VALUE)
                SetEvent(w.second.stop_event);
            if (w.second.thread.joinable())
                w.second.thread.join();
            if (w.second.dir_handle != INVALID_HANDLE_VALUE)
                CloseHandle(w.second.dir_handle);
        }
        watches.clear();
#elif defined(__APPLE__)
        if (stream) {
            FSEventStreamStop(stream);
            FSEventStreamRelease(stream);
            stream = nullptr;
        }
        if (thread.joinable())
            thread.join();
#elif defined(__linux__)
        if (inotify_fd >= 0) {
            close(inotify_fd);
            inotify_fd = -1;
        }
        if (thread.joinable())
            thread.join();
#endif
    }

    void on_changes(std::vector<FileChange> changes) {
        std::lock_guard<std::mutex> lock(pending_mutex);
        for (auto& c : changes) {
            pending.push_back(std::move(c));
        }
    }
};

FileWatcher::FileWatcher() : p_(std::make_unique<Impl>()) {}
FileWatcher::~FileWatcher() = default;

void FileWatcher::watch(const char* path) {
    watch({std::string(path)});
}

void FileWatcher::watch(const std::vector<std::string>& paths) {
#if defined(_WIN32)
    for (const auto& path_str : paths) {
        const std::string path = path_str;
        if (p_->watches.find(path) != p_->watches.end())
            continue;

        Impl::WinWatch ww;
        ww.dir_handle = CreateFileA(path.c_str(), GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
        if (ww.dir_handle == INVALID_HANDLE_VALUE)
            continue;

        ww.buffer.resize(kBufSize);
        ww.stop_event = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        ww.ol.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

        p_->watches[path] = std::move(ww);
        auto& w = p_->watches[path];
        auto path_copy = std::make_shared<std::string>(path);

        w.thread = std::thread([this, path_copy, &w]() {
            const char* path = path_copy->c_str();
            while (true) {
                DWORD bytes = 0;
                BOOL r = ReadDirectoryChangesW(w.dir_handle,
                    w.buffer.data(), static_cast<DWORD>(w.buffer.size()),
                    TRUE, notify_filters, &bytes, &w.ol, nullptr);

                HANDLE handles[2] = { w.ol.hEvent, w.stop_event };
                DWORD wait = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
                if (wait == WAIT_OBJECT_0 + 1)
                    break;

                if (r) {
                    std::vector<FileChange> changes;
                    FILE_NOTIFY_INFORMATION* info =
                        reinterpret_cast<FILE_NOTIFY_INFORMATION*>(w.buffer.data());
                    while (info) {
                        FileChange fc;
                        fc.mtime = get_file_mtime(path);
                        fc.size = 0;

                        int len = info->FileNameLength / 2;
                        std::wstring wname(info->FileName, info->FileName + len);
                        char name[MAX_PATH];
                        WideCharToMultiByte(CP_UTF8, 0, wname.c_str(), -1, name, MAX_PATH, nullptr, nullptr);

                        char full_path[MAX_PATH];
                        strcpy_s(full_path, path);
                        PathAppendA(full_path, name);
                        fc.path = full_path;
                        fc.mtime = get_file_mtime(full_path);
                        fc.size = get_file_size(full_path);

                        switch (info->Action) {
                            case FILE_ACTION_ADDED: fc.type = FileChangeType::Created; break;
                            case FILE_ACTION_MODIFIED: fc.type = FileChangeType::Modified; break;
                            case FILE_ACTION_REMOVED: fc.type = FileChangeType::Deleted; break;
                            case FILE_ACTION_RENAMED_OLD_NAME: fc.type = FileChangeType::Renamed; break;
                            case FILE_ACTION_RENAMED_NEW_NAME: continue;
                        }

                        changes.push_back(std::move(fc));
                        if (!info->NextEntryOffset) break;
                        info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                            reinterpret_cast<BYTE*>(info) + info->NextEntryOffset);
                    }
                    if (!changes.empty())
                        p_->on_changes(std::move(changes));
                }

                ResetEvent(w.ol.hEvent);
            }
        });
    }
#elif defined(__APPLE__)
    (void)paths;
#elif defined(__linux__)
    if (p_->inotify_fd < 0) {
        p_->inotify_fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
        if (p_->inotify_fd < 0)
            return;
    }
    for (const auto& path : paths) {
        int wd = inotify_add_watch(p_->inotify_fd, path.c_str(),
            IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM);
        if (wd >= 0) {
            p_->wd_to_path[wd] = path;
        }
    }
    if (!p_->thread.joinable()) {
        p_->thread = std::thread([this]() {
            char buf[8192];
            while (true) {
                ssize_t n = read(p_->inotify_fd, buf, sizeof(buf));
                if (n <= 0) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
                std::vector<FileChange> changes;
                inotify_event* ev = reinterpret_cast<inotify_event*>(buf);
                while (reinterpret_cast<char*>(ev) < buf + n) {
                    FileChange fc;
                    fc.path = p_->wd_to_path[ev->wd] + "/" + ev->name;
                    fc.mtime = get_file_mtime(fc.path.c_str());
                    fc.size = get_file_size(fc.path.c_str());
                    if (ev->mask & IN_CREATE) fc.type = FileChangeType::Created;
                    else if (ev->mask & IN_DELETE) fc.type = FileChangeType::Deleted;
                    else if (ev->mask & IN_MOVED_TO) fc.type = FileChangeType::Created;
                    else if (ev->mask & IN_MOVED_FROM) fc.type = FileChangeType::Renamed;
                    else fc.type = FileChangeType::Modified;
                    changes.push_back(std::move(fc));
                    ev = reinterpret_cast<inotify_event*>(
                        reinterpret_cast<char*>(ev) + sizeof(inotify_event) + ev->len);
                }
                if (!changes.empty())
                    p_->on_changes(std::move(changes));
            }
        });
    }
#endif
}

void FileWatcher::unwatch(const char* path) {
#if defined(_WIN32)
    auto it = p_->watches.find(path);
    if (it != p_->watches.end()) {
        if (it->second.stop_event != INVALID_HANDLE_VALUE)
            SetEvent(it->second.stop_event);
        if (it->second.thread.joinable())
            it->second.thread.join();
        if (it->second.dir_handle != INVALID_HANDLE_VALUE)
            CloseHandle(it->second.dir_handle);
        p_->watches.erase(it);
    }
#elif defined(__APPLE__)
    (void)path;
#elif defined(__linux__)
    for (auto it = p_->wd_to_path.begin(); it != p_->wd_to_path.end(); ) {
        if (it->second == path) {
            inotify_rm_watch(p_->inotify_fd, it->first);
            it = p_->wd_to_path.erase(it);
        } else {
            ++it;
        }
    }
#endif
}

void FileWatcher::unwatch_all() { p_->stop_all(); }

void FileWatcher::set_callback(Callback cb) { p_->callback = std::move(cb); }

void FileWatcher::poll() {
#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__linux__)
    for (const auto& path_str : p_->watch_paths) {
        WIN32_FILE_ATTRIBUTE_DATA attrs;
        if (!GetFileAttributesExA(path_str.c_str(), GetFileExInfoStandard, &attrs))
            continue;
        std::vector<FileChange> changes;
        FileChange fc;
        fc.path = path_str;
        fc.mtime = get_file_mtime(path_str.c_str());
        fc.size = get_file_size(path_str.c_str());
        fc.type = FileChangeType::Modified;
        changes.push_back(std::move(fc));
        if (!changes.empty())
            p_->on_changes(std::move(changes));
    }
#else
    (void)0;
#endif
}

std::vector<FileChange> FileWatcher::flush() {
    std::lock_guard<std::mutex> lock(p_->pending_mutex);
    std::vector<FileChange> result = std::move(p_->pending);
    p_->pending.clear();
    return result;
}

} // namespace hotreload
} // namespace q3d
