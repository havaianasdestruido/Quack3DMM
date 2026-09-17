#include "script_manager.h"

#include <cstring>

namespace q3d {
namespace script {

void ScriptManager::init() {}

void ScriptManager::shutdown() {
    for (auto& rt : runtimes_) {
        if (rt.runtime) rt.runtime->shutdown();
    }
}

void ScriptManager::register_runtime(const char *name, IRuntime* rt) {
    if (!name || !rt) return;
    runtimes_.push_back({ std::string(name), rt });
}

IRuntime* ScriptManager::get_runtime(const char *name) {
    if (!name) return nullptr;
    for (auto& rt : runtimes_) {
        if (rt.name == name) {
            return rt.runtime;
        }
    }
    return nullptr;
}

void ScriptManager::load_mod_scripts(const char *mod_id, const ModManifest& manifest) {
    for (auto& rt : runtimes_) {
        if (rt.runtime) rt.runtime->load_mod_scripts(mod_id, manifest);
    }
}

void ScriptManager::unload_mod_scripts(const char *mod_id) {
    for (auto& rt : runtimes_) {
        if (rt.runtime) rt.runtime->unload_mod_scripts(mod_id);
    }
}

void ScriptManager::reload_mod_scripts(const char *mod_id) {
    for (auto& rt : runtimes_) {
        if (rt.runtime) rt.runtime->reload_mod_scripts(mod_id);
    }
}

void ScriptManager::on_frame(float dt) {
    for (auto& rt : runtimes_) {
        if (rt.runtime) rt.runtime->on_frame(dt);
    }
}

void ScriptManager::on_event(const char *event_name, const VariantList& args) {
    for (auto& rt : runtimes_) {
        if (rt.runtime) rt.runtime->on_event(event_name, args);
    }
}

void ScriptManager::list_all_scripts(std::vector<ScriptInfo>& out) {
    for (auto& rt : runtimes_) {
        if (rt.runtime) rt.runtime->list_all_scripts(out);
    }
}

} // namespace script
} // namespace q3d
