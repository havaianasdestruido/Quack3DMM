#include "actor_system.h"

namespace q3d {
namespace actor {

ActorRegistry& ActorRegistry::instance() {
    static ActorRegistry instance;
    return instance;
}

void ActorRegistry::register_actor(shared_ptr<ActorDefinition> def) {
    actors_[def->id] = def;
}

void ActorRegistry::unregister_actor(StrPtr id) {
    actors_.erase(id);
}

shared_ptr<ActorDefinition> ActorRegistry::get(StrPtr id) const {
    auto it = actors_.find(id);
    return it != actors_.end() ? it->second : nullptr;
}

vector<shared_ptr<ActorDefinition>> ActorRegistry::find(
    const char* tag,
    const char* name_contains,
    const char* author
) const {
    vector<shared_ptr<ActorDefinition>> result;
    for (const auto& [id, def] : actors_) {
        bool match = true;
        if (tag) {
            match = false;
            for (const auto& t : def->tags) {
                if (t == tag) { match = true; break; }
            }
        }
        if (match && name_contains) {
            match = def->name.find(name_contains) != string::npos;
        }
        if (match && author) {
            match = def->author == author;
        }
        if (match) result.push_back(def);
    }
    return result;
}

const unordered_map<string, shared_ptr<ActorDefinition>>& ActorRegistry::all() const {
    return actors_;
}

shared_ptr<BodyHandle> ActorRegistry::instantiate_body(const ActorDefinition& def) const {
    (void)def;
    return nullptr;
}

void ActorRegistry::instantiate_actor(const ActorDefinition& def, const char* instance_id) const {
    (void)def;
    (void)instance_id;
}

void ActorRegistry::hot_reload() {
}

vector<CostumeSlot> CostumeManager::get_costumes_for_slot(BodySlot slot) const {
    vector<CostumeSlot> result;
    for (const auto& [id, def] : ActorRegistry::instance().all()) {
        for (const auto& cs : def->default_costume) {
            if (cs.slot == slot) result.push_back(cs);
        }
    }
    return result;
}

void CostumeManager::apply_costume(BodyHandle body, const vector<CostumeSlot>& costume) {
    (void)body;
    (void)costume;
}

bool CostumeManager::validate_compatibility(const ActorDefinition& actor, const CostumeSlot& costume) const {
    (void)actor;
    (void)costume;
    return true;
}

} // namespace actor
} // namespace q3d