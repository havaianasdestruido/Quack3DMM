#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

#include "file_watcher.h"

namespace q3d {
namespace hotreload {

using StrPtr = const char*;

enum class ReloadType {
    ScriptLua,
    ScriptPython,
    ScriptWasm,
    Texture,
    Model,
    Material,
    Shader,
    UIPanel,
    Scene,
    Sound,
    Font,
    Config,
};

struct ReloadTask {
    std::string vpath;
    ReloadType type;
    long mtime;
    int priority;
    std::string mod_id;
};

struct ScriptState {
    std::string mod_id;
    std::unordered_map<std::string, int> variables_int;
    std::unordered_map<std::string, double> variables_float;
    std::unordered_map<std::string, std::string> variables_string;
    std::string suspended_at;
};

class ReloadManager {
public:
    static ReloadManager& instance();

    void register_mod(StrPtr mod_id, const std::vector<std::string>& watch_patterns);
    void unregister_mod(StrPtr mod_id);

    void reload(StrPtr vpath, ReloadType type);
    void reload_mod(StrPtr mod_id);
    void reload_all();

    void on_texture_changed(StrPtr vpath);
    void on_model_changed(StrPtr vpath);
    void on_shader_changed(StrPtr vpath);
    void on_script_changed(StrPtr vpath);
    void on_ui_changed(StrPtr vpath);
    void on_scene_changed(StrPtr vpath);
    void on_sound_changed(StrPtr vpath);
    void on_config_changed(StrPtr vpath);

    void snapshot_script_state(StrPtr mod_id);
    void restore_script_state(StrPtr mod_id);

    void defer_reload(const ReloadTask& task);
    void defer_reload_immediate(const ReloadTask& task);

    void tick();

    void flush();

    struct Metrics {
        int reloads_this_frame;
        int pending_count;
        float avg_reload_time_ms;
        int total_reloads;
    };
    Metrics metrics() const;

    std::vector<ReloadTask> pending_reloads() const;

    void set_manual_hotkey(int vk);

    void add_ignore_pattern(const char* pattern);

private:
    ReloadManager();

    FileWatcher watcher_;
    std::vector<ReloadTask> pending_;
    std::vector<ReloadTask> immediate_;
    std::unordered_map<std::string, ScriptState> script_states_;
    std::unordered_set<std::string> ignore_patterns_;
    mutable std::mutex mutex_;
    int manual_hotkey_ = 0;

    void process_reload(const ReloadTask& task);
};

} // namespace hotreload
} // namespace q3d
