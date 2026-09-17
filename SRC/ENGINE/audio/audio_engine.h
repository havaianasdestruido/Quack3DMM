#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace q3d {
namespace audio {

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

class DSPNode;

// Audio buses/channels
enum class Bus {
    Master,
    Music,
    SFX,
    Voice,
    Ambient,
    UI,
    Custom,
};

class AudioEngine {
public:
    static AudioEngine &instance();

    void init();
    void shutdown();

    void update(float dt);

    void set_listener(const Vec3 &pos, const Vec3 &forward, const Vec3 &up);

    void set_bus_volume(Bus bus, float volume);
    float get_bus_volume(Bus bus) const;
    void set_bus_mute(Bus bus, bool mute);
    bool get_bus_mute(Bus bus) const;

    int create_bus(const string &name);
    void destroy_bus(int bus_id);

    void add_bus_effect(Bus bus, shared_ptr<DSPNode> effect);
    void remove_bus_effect(Bus bus, shared_ptr<DSPNode> effect);
    void set_bus_effect_param(Bus bus, int effect_index, const char *param, float value);

    void set_voice_count(int max_voices);

private:
    struct BusState {
        string name;
        float volume = 1.0f;
        bool mute = false;
        vector<shared_ptr<DSPNode>> effects;
    };

    AudioEngine();

    BusState &get_bus_state(Bus bus);
    const BusState &get_bus_state(Bus bus) const;

    Vec3 listener_pos_{};
    Vec3 listener_forward_{};
    Vec3 listener_up_{};

    unordered_map<int, BusState> custom_buses_;
    int next_custom_bus_id_ = 0;

    float master_volume_ = 1.0f;
    float music_volume_ = 1.0f;
    float sfx_volume_ = 1.0f;
    float voice_volume_ = 1.0f;
    float ambient_volume_ = 1.0f;
    float ui_volume_ = 1.0f;

    bool master_mute_ = false;
    bool music_mute_ = false;
    bool sfx_mute_ = false;
    bool voice_mute_ = false;
    bool ambient_mute_ = false;
    bool ui_mute_ = false;

    int max_voices_ = 64;
};

} // namespace audio
} // namespace q3d