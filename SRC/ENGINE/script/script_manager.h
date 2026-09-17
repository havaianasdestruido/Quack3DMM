#pragma once

#include <string>
#include <vector>

namespace q3d {
namespace script {

struct ScriptInfo
{
    std::string runtime;
    std::string mod_id;
    std::string path;
};

struct VariantList
{
    // Placeholder for future variant implementation
};

struct ModManifest;

class IRuntime
{
public:
    virtual ~IRuntime() = default;

    virtual void init() = 0;
    virtual void shutdown() = 0;

    virtual void load_mod_scripts(const char *mod_id, const ModManifest& manifest) = 0;
    virtual void unload_mod_scripts(const char *mod_id) = 0;
    virtual void reload_mod_scripts(const char *mod_id) = 0;

    virtual void on_frame(float dt) = 0;
    virtual void on_event(const char *event_name, const VariantList& args) = 0;

    virtual void list_all_scripts(std::vector<ScriptInfo>& out) = 0;
};

class ScriptManager
{
public:
    void init();
    void shutdown();

    void register_runtime(const char *name, IRuntime* rt);
    IRuntime* get_runtime(const char *name);

    void load_mod_scripts(const char *mod_id, const ModManifest& manifest);
    void unload_mod_scripts(const char *mod_id);
    void reload_mod_scripts(const char *mod_id);

    void on_frame(float dt);
    void on_event(const char *event_name, const VariantList& args);

    void list_all_scripts(std::vector<ScriptInfo>& out);

private:
    struct RuntimeEntry
    {
        std::string name;
        IRuntime* runtime;
    };

    std::vector<RuntimeEntry> runtimes_;
};

} // namespace script
} // namespace q3d
