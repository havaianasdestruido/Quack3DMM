#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace q3d {
namespace mod {

using std::string;
using std::vector;
using std::unordered_map;
using std::unique_ptr;
using std::shared_ptr;

using StrPtr = const char*;

struct Version {
    int major = 0, minor = 0, patch = 0;
    string prerelease;

    static Version parse(StrPtr s);
    bool satisfies(const char* range) const;
    string to_string() const;
    int compare(const Version& other) const;
};

enum class ModFlag {
    Experimental,
    Deprecated,
    ServerOnly,
    ClientOnly,
    EditorOnly,
    RuntimeOnly,
    Lazy,
};

enum class TrustTier {
    Core,
    Workshop,
    Local,
    Dev,
};

enum class ContentRating {
    Safe,
    Mature,
    Explicit,
};

struct Dependency {
    string mod_id;
    string version_range;
    bool optional = false;
};

struct Conflict {
    string mod_id;
    string reason;
};

struct ModPermission {
    string name;
    bool requested = false;
};

struct Localization {
    string locale;
    string name;
    string description;
};

struct EntryPoint {
    string type;
    string path;
    string when;
};

struct AssetPattern {
    string pattern;
    string category;
};

struct Patch {
    string target_file;
    string patch_file;
    string type;
};

struct ModManifest {
    int schema_version = 2;
    string id;
    string name;
    string version;
    vector<string> authors;
    string description;
    vector<string> tags;
    string engine_version;
    vector<Dependency> dependencies;
    vector<Conflict> conflicts;
    vector<string> load_after;
    vector<string> load_before;
    TrustTier trust_tier = TrustTier::Workshop;
    ContentRating content_rating = ContentRating::Safe;
    vector<ModFlag> flags;
    int memory_budget_mb = 256;
    string runtime;
    vector<ModPermission> permissions;
    bool sandbox = true;
    vector<AssetPattern> assets;
    vector<Patch> patches;
    vector<EntryPoint> entry_points;
    vector<Localization> localizations;
    bool preload_textures = false;
    bool preload_models = false;
    string variant;
    string source_dir;
};

class ManifestParser {
public:
    static ModManifest parse(const char* toml_content, StrPtr source = "");
    static ModManifest parse_file(StrPtr path);
    static vector<string> validate(const ModManifest& m);
    static vector<string> lint(const char* toml_content);

private:
    static void lint_typos(const char* key, const vector<string>& known, vector<string>& errors);
    static string get_string(const unordered_map<string, string>& table, const string& key, const string& def = "");
    static int get_int(const unordered_map<string, string>& table, const string& key, int def = 0);
    static bool get_bool(const unordered_map<string, string>& table, const string& key, bool def = false);
};

class ManifestWriter {
public:
    static string to_toml(const ModManifest& m);
    static bool write(const ModManifest& m, StrPtr path);
};

} // namespace mod
} // namespace q3d
