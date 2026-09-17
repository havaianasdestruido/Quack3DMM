#include "reload_manager.h"

#include <algorithm>
#include <cctype>
#include <chrono>

namespace q3d {
namespace hotreload {

ReloadManager& ReloadManager::instance() {
    static ReloadManager r;
    return r;
}

ReloadManager::ReloadManager() = default;

void ReloadManager::register_mod(StrPtr mod_id, const std::vector<std::string>& watch_patterns) {
    if (!mod_id) return;
    std::lock_guard<std::mutex> lock(mutex_);
    (void)watch_patterns;
    // Mark this mod id as needing state snapshot bookkeeping (first load).
    script_states_.emplace(mod_id, ScriptState{})
        .first->second.mod_id = mod_id;
}

void ReloadManager::unregister_mod(StrPtr mod_id) {
    if (!mod_id) return;
    std::lock_guard<std::mutex> lock(mutex_);
    script_states_.erase(mod_id);
}

void ReloadManager::reload(StrPtr vpath, ReloadType type) {
    if (!vpath) return;
    ReloadTask task;
    task.vpath = vpath;
    task.type = type;
    task.mtime = 0;
    task.priority = 0;
    task.mod_id = "";
    defer_reload_immediate(task);
}

void ReloadManager::reload_mod(StrPtr mod_id) {
    if (!mod_id) return;
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& t : immediate_) {
        if (t.mod_id == mod_id)
            defer_reload_immediate(t);
    }
    for (auto& t : pending_) {
        if (t.mod_id == mod_id)
            defer_reload_immediate(t);
    }
}

void ReloadManager::reload_all() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto snapshot = pending_;
    pending_.clear();
    for (const auto& t : snapshot)
        immediate_.push_back(t);
}

void ReloadManager::on_texture_changed(StrPtr vpath) {
    reload(vpath, ReloadType::Texture);
}

void ReloadManager::on_model_changed(StrPtr vpath) {
    reload(vpath, ReloadType::Model);
}

void ReloadManager::on_shader_changed(StrPtr vpath) {
    reload(vpath, ReloadType::Shader);
}

void ReloadManager::on_script_changed(StrPtr vpath) {
    reload(vpath, ReloadType::ScriptLua);
}

void ReloadManager::on_ui_changed(StrPtr vpath) {
    reload(vpath, ReloadType::UIPanel);
}

void ReloadManager::on_scene_changed(StrPtr vpath) {
    reload(vpath, ReloadType::Scene);
}

void ReloadManager::on_sound_changed(StrPtr vpath) {
    reload(vpath, ReloadType::Sound);
}

void ReloadManager::on_config_changed(StrPtr vpath) {
    reload(vpath, ReloadType::Config);
}

void ReloadManager::snapshot_script_state(StrPtr mod_id) {
    if (!mod_id) return;
    // Snapshot preserves script-scoped variables across a reload.
    // Implemented as a marker so restore can re-create the entry.
    script_states_[mod_id].suspended_at = "snapshotted";
}

void ReloadManager::restore_script_state(StrPtr mod_id) {
    if (!mod_id) return;
    auto it = script_states_.find(mod_id);
    if (it != script_states_.end())
        it->second.suspended_at.clear();
}

void ReloadManager::defer_reload(const ReloadTask& task) {
    std::lock_guard<std::mutex> lock(mutex_);
    pending_.push_back(task);
}

void ReloadManager::defer_reload_immediate(const ReloadTask& task) {
    std::lock_guard<std::mutex> lock(mutex_);
    immediate_.push_back(task);
}

void ReloadManager::tick() {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    long ms = static_cast<long>(std::chrono::duration_cast<std::chrono::milliseconds>(now).count());

    // Drain watcher changes into deferred queue.
    auto changes = watcher_.flush();
    for (auto& c : changes) {
        if (c.path.empty())
            continue;
        // Apply ignore patterns.
        bool ignored = false;
        for (const auto& pat : ignore_patterns_) {
            if (!pat.empty() && c.path.find(pat) != std::string::npos) {
                ignored = true;
                break;
            }
        }
        if (ignored)
            continue;

        ReloadTask task;
        task.vpath = c.path;
        task.mtime = c.mtime;
        task.mod_id = c.mod_id;
        task.priority = 0;

        std::string lower = c.path;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        if (lower.find(".lua") != std::string::npos) task.type = ReloadType::ScriptLua;
        else if (lower.find(".py") != std::string::npos) task.type = ReloadType::ScriptPython;
        else if (lower.find(".wasm") != std::string::npos) task.type = ReloadType::ScriptWasm;
        else if (lower.find(".png") != std::string::npos || lower.find(".jpg") != std::string::npos ||
                 lower.find(".bmp") != std::string::npos) task.type = ReloadType::Texture;
        else if (lower.find(".obj") != std::string::npos || lower.find(".fbx") != std::string::npos ||
                 lower.find(".gltf") != std::string::npos || lower.find(".glb") != std::string::npos) task.type = ReloadType::Model;
        else if (lower.find(".toml") != std::string::npos || lower.find(".json") != std::string::npos ||
                 lower.find(".cfg") != std::string::npos) task.type = ReloadType::Config;
        else if (lower.find(".glsl") != std::string::npos || lower.find(".hlsl") != std::string::npos ||
                 lower.find(".wgsl") != std::string::npos) task.type = ReloadType::Shader;
        else if (lower.find(".3mm") != std::string::npos) task.type = ReloadType::Scene;
        else if (lower.find(".wav") != std::string::npos || lower.find(".mp3") != std::string::npos ||
                 lower.find(".ogg") != std::string::npos) task.type = ReloadType::Sound;
        else task.type = ReloadType::Config;

        pending_.push_back(std::move(task));
    }

    (void)ms;
}

void ReloadManager::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& t : immediate_)
        process_reload(t);
    immediate_.clear();
}

ReloadManager::Metrics ReloadManager::metrics() const {
    Metrics m;
    m.reloads_this_frame = static_cast<int>(immediate_.size());
    m.pending_count = static_cast<int>(pending_.size());
    m.avg_reload_time_ms = 0.0f;
    m.total_reloads = 0;
    return m;
}

std::vector<ReloadTask> ReloadManager::pending_reloads() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pending_;
}

void ReloadManager::set_manual_hotkey(int vk) {
    manual_hotkey_ = vk;
}

void ReloadManager::add_ignore_pattern(const char* pattern) {
    if (pattern)
        ignore_patterns_.insert(pattern);
}

void ReloadManager::process_reload(const ReloadTask& task) {
    (void)task;
    // Dispatch point for per-type reload handlers.
    // Handled by asset_reloader / script manager integration in host app.
}

} // namespace hotreload
} // namespace q3d