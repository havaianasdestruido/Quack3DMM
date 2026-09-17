#include "mod_manifest.h"

#include <fstream>
#include <sstream>

namespace q3d {
namespace mod {

Version Version::parse(StrPtr s) {
    Version v;
    if (!s) return v;
    std::string str(s);
    auto dash = str.find('-');
    std::string core = dash == std::string::npos ? str : str.substr(0, dash);
    if (dash != std::string::npos) v.prerelease = str.substr(dash + 1);
    int parts[3] = {0, 0, 0};
    size_t start = 0; int idx = 0;
    while (start < core.size() && idx < 3) {
        auto dot = core.find('.', start);
        auto token = core.substr(start, dot == std::string::npos ? core.size() - start : dot - start);
        parts[idx++] = std::stoi(token);
        if (dot == std::string::npos) break;
        start = dot + 1;
    }
    v.major = parts[0];
    v.minor = parts[1];
    v.patch = parts[2];
    return v;
}

int Version::compare(const Version& other) const {
    if (major != other.major) return major < other.major ? -1 : 1;
    if (minor != other.minor) return minor < other.minor ? -1 : 1;
    if (patch != other.patch) return patch < other.patch ? -1 : 1;
    return 0;
}

bool Version::satisfies(const char* range) const {
    if (!range || !*range) return true;
    std::string r(range);
    std::istringstream iss(r);
    std::string token;
    bool ok = true;
    while (iss >> token) {
        char op1 = 0, op2 = 0;
        size_t pos = 0;
        if (token[0] == '^' || token[0] == '~') {
            op1 = token[0];
            pos = 1;
        } else if (token[0] == '>' || token[0] == '<' || token[0] == '=') {
            op1 = token[0];
            pos = 1;
            if (token.size() > 1 && (token[1] == '=')) { op2 = token[1]; pos = 2; }
        }
        Version v = Version::parse(token.c_str() + pos);
        if (op1 == '^') {
            Version min = v;
            Version max = v; max.major += 1; max.minor = 0; max.patch = 0;
            ok = compare(min) >= 0 && compare(max) < 0;
        } else if (op1 == '~') {
            Version min = v;
            Version max = v; max.minor += 1; max.patch = 0;
            ok = compare(min) >= 0 && compare(max) < 0;
        } else if (op1 == '>' && op2 == '=') {
            ok = compare(v) >= 0;
        } else if (op1 == '<' && op2 == '=') {
            ok = compare(v) <= 0;
        } else if (op1 == '>') {
            ok = compare(v) > 0;
        } else if (op1 == '<') {
            ok = compare(v) < 0;
        } else if (op1 == '=') {
            ok = compare(v) == 0;
        } else {
            ok = compare(v) == 0;
        }
        if (!ok) return false;
    }
    return true;
}

std::string Version::to_string() const {
    std::string s = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    if (!prerelease.empty()) s += "-" + prerelease;
    return s;
}

ModManifest ManifestParser::parse(const char* toml_content, StrPtr source) {
    ModManifest m;
    m.source_dir = source ? source : "";
    // ponytail: minimal parser only handles flat key=value pairs and simple arrays.
    std::istringstream iss(toml_content ? toml_content : "");
    std::string line;
    std::unordered_map<std::string, std::string> scalars;
    while (std::getline(iss, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        auto trim = [](std::string& s) {
            size_t b = s.find_first_not_of(" \t\r");
            size_t e = s.find_last_not_of(" \t\r");
            if (b == std::string::npos) { s.clear(); return; }
            s = s.substr(b, e - b + 1);
            if (s.size() >= 2 && s.front() == '"' && s.back() == '"') s = s.substr(1, s.size() - 2);
        };
        trim(key);
        trim(val);
        scalars[key] = val;
    }
    m.id = get_string(scalars, "id");
    m.name = get_string(scalars, "name");
    m.version = get_string(scalars, "version");
    m.description = get_string(scalars, "description");
    m.engine_version = get_string(scalars, "engine_version");
    m.runtime = get_string(scalars, "runtime");
    m.schema_version = get_int(scalars, "schema_version", 2);
    m.sandbox = get_bool(scalars, "sandbox", true);
    m.preload_textures = get_bool(scalars, "preload_textures", false);
    m.preload_models = get_bool(scalars, "preload_models", false);
    m.memory_budget_mb = get_int(scalars, "memory_budget_mb", 256);
    m.variant = get_string(scalars, "variant");
    return m;
}

ModManifest ManifestParser::parse_file(StrPtr path) {
    std::ifstream f(path, std::ios::binary);
    std::stringstream ss;
    ss << f.rdbuf();
    return parse(ss.str().c_str(), path);
}

std::vector<std::string> ManifestParser::validate(const ModManifest& m) {
    std::vector<std::string> errs;
    if (m.id.empty()) errs.push_back("manifest.id is required");
    if (m.version.empty()) errs.push_back("manifest.version is required");
    if (m.engine_version.empty()) errs.push_back("manifest.engine_version is required");
    return errs;
}

std::vector<std::string> ManifestParser::lint(const char* toml_content) {
    std::vector<std::string> msgs;
    std::istringstream iss(toml_content ? toml_content : "");
    std::string line;
    while (std::getline(iss, line)) {
        if (line.find("dependencys") != std::string::npos) {
            msgs.push_back("you wrote 'dependencys', did you mean 'dependencies'?");
        }
        if (line.find("permssions") != std::string::npos) {
            msgs.push_back("you wrote 'permssions', did you mean 'permissions'?");
        }
    }
    return msgs;
}

void ManifestParser::lint_typos(const char* key, const std::vector<std::string>& known, std::vector<std::string>& errors) {
    std::string k(key ? key : "");
    for (auto& ref : known) {
        if (k.size() == ref.size() + 1 && k.find(ref) != std::string::npos) {
            errors.push_back("you wrote '" + k + "', did you mean '" + ref + "'?");
            return;
        }
    }
}

std::string ManifestParser::get_string(const std::unordered_map<std::string, std::string>& table, const std::string& key, const std::string& def) {
    auto it = table.find(key);
    return it == table.end() ? def : it->second;
}

int ManifestParser::get_int(const std::unordered_map<std::string, std::string>& table, const std::string& key, int def) {
    auto it = table.find(key);
    if (it == table.end() || it->second.empty()) return def;
    return std::stoi(it->second);
}

bool ManifestParser::get_bool(const std::unordered_map<std::string, std::string>& table, const std::string& key, bool def) {
    auto it = table.find(key);
    if (it == table.end()) return def;
    return it->second == "true" || it->second == "1";
}

std::string ManifestWriter::to_toml(const ModManifest& m) {
    std::ostringstream os;
    os << "schema_version = " << m.schema_version << "\n";
    if (!m.id.empty()) os << "id = \"" << m.id << "\"\n";
    if (!m.name.empty()) os << "name = \"" << m.name << "\"\n";
    if (!m.version.empty()) os << "version = \"" << m.version << "\"\n";
    if (!m.description.empty()) os << "description = \"" << m.description << "\"\n";
    if (!m.engine_version.empty()) os << "engine_version = \"" << m.engine_version << "\"\n";
    if (!m.runtime.empty()) os << "runtime = \"" << m.runtime << "\"\n";
    os << "sandbox = " << (m.sandbox ? "true" : "false") << "\n";
    os << "memory_budget_mb = " << m.memory_budget_mb << "\n";
    os << "preload_textures = " << (m.preload_textures ? "true" : "false") << "\n";
    os << "preload_models = " << (m.preload_models ? "true" : "false") << "\n";
    if (!m.variant.empty()) os << "variant = \"" << m.variant << "\"\n";
    return os.str();
}

bool ManifestWriter::write(const ModManifest& m, StrPtr path) {
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    std::string s = to_toml(m);
    f.write(s.data(), static_cast<std::streamsize>(s.size()));
    return static_cast<bool>(f);
}

} // namespace mod
} // namespace q3d
