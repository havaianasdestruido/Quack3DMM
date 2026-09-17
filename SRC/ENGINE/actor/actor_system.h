#pragma once

#include "actor_def.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace q3d {
namespace actor {

struct Body;
using BodyHandle = Body*;

class ActorRegistry {
public:
    static ActorRegistry& instance();

    void register_actor(shared_ptr<ActorDefinition> def);
    void unregister_actor(StrPtr id);
    shared_ptr<ActorDefinition> get(StrPtr id) const;
    vector<shared_ptr<ActorDefinition>> find(
        const char* tag = nullptr,
        const char* name_contains = nullptr,
        const char* author = nullptr
    ) const;
    const unordered_map<string, shared_ptr<ActorDefinition>>& all() const;

    shared_ptr<BodyHandle> instantiate_body(const ActorDefinition& def) const;
    void instantiate_actor(const ActorDefinition& def, const char* instance_id) const;

    void hot_reload();

private:
    ActorRegistry() = default;
    unordered_map<string, shared_ptr<ActorDefinition>> actors_;
    unordered_map<string, long> file_mtimes_;
};

class CostumeManager {
public:
    vector<CostumeSlot> get_costumes_for_slot(BodySlot slot) const;
    void apply_costume(BodyHandle body, const vector<CostumeSlot>& costume);
    bool validate_compatibility(const ActorDefinition& actor, const CostumeSlot& costume) const;
};

} // namespace actor
} // namespace q3d