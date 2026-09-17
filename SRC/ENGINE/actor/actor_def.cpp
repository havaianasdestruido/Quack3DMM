#include "actor_def.h"

#include <cstdio>
#include <cstring>
#include <string>

namespace q3d {
namespace actor {

namespace {

void trim_newline(string& s) {
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
        s.pop_back();
}

} // namespace

shared_ptr<ActorDefinition> ActorDefinitionLoader::load(const char* vpath) {
    if (!vpath) return nullptr;
    FILE* f = fopen(vpath, "rb");
    if (!f) return nullptr;
    std::string data;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
        data.append(buf, n);
    fclose(f);
    return load(data.data(), data.size(), "txt");
}

shared_ptr<ActorDefinition> ActorDefinitionLoader::load(const char* data, size_t len, const char* format) {
    (void)format;
    if (!data || len == 0) return nullptr;
    auto def = std::make_shared<ActorDefinition>();
    std::string s(data, len);
    size_t pos = 0;
    while (pos < s.size()) {
        size_t eol = s.find('\n', pos);
        if (eol == std::string::npos) eol = s.size();
        std::string line = s.substr(pos, eol - pos);
        pos = (eol == s.size()) ? eol : eol + 1;
        trim_newline(line);
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        if (key == "id") def->id = val;
        else if (key == "name") def->name = val;
        else if (key == "version") def->version = val;
        else if (key == "main_model") def->main_model = val;
        else if (key == "head_model") def->head_model = val;
        else if (key == "body_model") def->body_model = val;
        else if (key == "author") def->author = val;
        else if (key == "description") def->description = val;
        else if (key == "thumbnail") def->thumbnail = val;
        else if (key == "rig_type") def->rig.type = val;
        else if (key == "rig_retarget_from") def->rig.retarget_from = val;
        else if (key == "default_animation") def->default_animation = val;
        else if (key == "tag") def->tags.push_back(val);
        else def->custom[key] = val;
    }
    return def;
}

bool ActorDefinitionLoader::save(const ActorDefinition& def, const char* vpath) {
    if (!vpath) return false;
    FILE* f = fopen(vpath, "wb");
    if (!f) return false;
    auto put = [f](const char* key, const string& val) {
        if (val.empty()) return;
        fprintf(f, "%s=%s\n", key, val.c_str());
    };
    put("id", def.id);
    put("name", def.name);
    put("version", def.version);
    put("main_model", def.main_model);
    put("head_model", def.head_model);
    put("body_model", def.body_model);
    put("author", def.author);
    put("description", def.description);
    put("thumbnail", def.thumbnail);
    put("rig_type", def.rig.type);
    put("rig_retarget_from", def.rig.retarget_from);
    put("default_animation", def.default_animation);
    for (const auto& t : def.tags)
        fprintf(f, "tag=%s\n", t.c_str());
    bool ok = fclose(f) == 0;
    return ok;
}

} // namespace actor
} // namespace q3d