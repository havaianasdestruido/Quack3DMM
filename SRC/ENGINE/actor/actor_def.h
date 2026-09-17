#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace q3d {
namespace actor {

using std::string;
using std::vector;
using std::unordered_map;
using std::shared_ptr;

using StrPtr = const char*;

enum class BodySlot {
    Root,
    Head, Hair, Hat, Mask,
    Neck, Chest, Back, Cape, Wings,
    LeftShoulder, RightShoulder,
    LeftUpperArm, LeftForearm, LeftHand, LeftAccessory,
    RightUpperArm, RightForearm, RightHand, RightAccessory,
    Waist, Hips, LeftThigh, LeftCalf, LeftFoot, LeftShoe,
    RightThigh, RightCalf, RightFoot, RightShoe,
    Tail, Aura, Custom1, Custom2, Custom3, Custom4, Custom5,
    Count
};

struct CostumeSlot {
    BodySlot slot;
    string model_path;
    string texture_override;
    string material_override;
    bool hidden = false;
};

struct AnimMapping {
    string name;
    string anim_path;
    float blend_in = 0.2f;
    float blend_out = 0.2f;
};

struct VoiceSlot {
    string phoneme_map;
    string default_voice;
    string language;
};

struct PersonalityTrait {
    string emotion;
    string prompt;
    string expression;
};

struct ActorTrait {
    bool can_swim = false;
    bool can_fly = false;
    bool has_wings = false;
    bool cast_shadows = true;
    bool has_ragdoll = false;
    bool is_humanoid = true;
    float mass = 1.0f;
    float height = 1.8f;
};

struct RigDefinition {
    string type;
    vector<string> bone_names;
    unordered_map<string, string> bone_aliases;
    string retarget_from;
};

struct ActorDefinition {
    string id;
    string name;
    string version = "1.0.0";

    string main_model;
    string head_model;
    string body_model;
    unordered_map<string, string> slot_models;

    RigDefinition rig;

    vector<AnimMapping> animations;
    string default_animation = "idle";

    vector<CostumeSlot> default_costume;

    vector<VoiceSlot> voice_slots;

    vector<PersonalityTrait> personality_traits;

    ActorTrait traits;

    string author;
    string description;
    string thumbnail;
    vector<string> tags;

    unordered_map<string, string> custom;
};

class ActorDefinitionLoader {
public:
    static shared_ptr<ActorDefinition> load(const char* vpath);
    static shared_ptr<ActorDefinition> load(const char* data, size_t len, const char* format);
    static bool save(const ActorDefinition& def, const char* vpath);
};

} // namespace actor
} // namespace q3d
