#include "audio_engine.h"
#include "dsp.h"

namespace q3d {
namespace audio {

AudioEngine &AudioEngine::instance()
{
    static AudioEngine inst;
    return inst;
}

AudioEngine::AudioEngine() = default;

void AudioEngine::init()
{
}

void AudioEngine::shutdown()
{
}

void AudioEngine::update(float /*dt*/)
{
}

void AudioEngine::set_listener(const Vec3 &pos, const Vec3 &forward, const Vec3 &up)
{
    listener_pos_ = pos;
    listener_forward_ = forward;
    listener_up_ = up;
}

AudioEngine::BusState &AudioEngine::get_bus_state(Bus bus)
{
    switch (bus) {
    case Bus::Master: {
        static BusState s{"master"};
        s.volume = master_volume_;
        s.mute = master_mute_;
        return s;
    }
    case Bus::Music: {
        static BusState s{"music"};
        s.volume = music_volume_;
        s.mute = music_mute_;
        return s;
    }
    case Bus::SFX: {
        static BusState s{"sfx"};
        s.volume = sfx_volume_;
        s.mute = sfx_mute_;
        return s;
    }
    case Bus::Voice: {
        static BusState s{"voice"};
        s.volume = voice_volume_;
        s.mute = voice_mute_;
        return s;
    }
    case Bus::Ambient: {
        static BusState s{"ambient"};
        s.volume = ambient_volume_;
        s.mute = ambient_mute_;
        return s;
    }
    case Bus::UI: {
        static BusState s{"ui"};
        s.volume = ui_volume_;
        s.mute = ui_mute_;
        return s;
    }
    case Bus::Custom:
    default: {
        static BusState s{"custom"};
        return s;
    }
    }
}

const AudioEngine::BusState &AudioEngine::get_bus_state(Bus bus) const
{
    return const_cast<AudioEngine *>(this)->get_bus_state(bus);
}

void AudioEngine::set_bus_volume(Bus bus, float volume)
{
    switch (bus) {
    case Bus::Master: master_volume_ = volume; break;
    case Bus::Music: music_volume_ = volume; break;
    case Bus::SFX: sfx_volume_ = volume; break;
    case Bus::Voice: voice_volume_ = volume; break;
    case Bus::Ambient: ambient_volume_ = volume; break;
    case Bus::UI: ui_volume_ = volume; break;
    case Bus::Custom:
    default: break;
    }
}

float AudioEngine::get_bus_volume(Bus bus) const
{
    switch (bus) {
    case Bus::Master: return master_volume_;
    case Bus::Music: return music_volume_;
    case Bus::SFX: return sfx_volume_;
    case Bus::Voice: return voice_volume_;
    case Bus::Ambient: return ambient_volume_;
    case Bus::UI: return ui_volume_;
    case Bus::Custom:
    default: return 1.0f;
    }
}

void AudioEngine::set_bus_mute(Bus bus, bool mute)
{
    switch (bus) {
    case Bus::Master: master_mute_ = mute; break;
    case Bus::Music: music_mute_ = mute; break;
    case Bus::SFX: sfx_mute_ = mute; break;
    case Bus::Voice: voice_mute_ = mute; break;
    case Bus::Ambient: ambient_mute_ = mute; break;
    case Bus::UI: ui_mute_ = mute; break;
    case Bus::Custom:
    default: break;
    }
}

bool AudioEngine::get_bus_mute(Bus bus) const
{
    switch (bus) {
    case Bus::Master: return master_mute_;
    case Bus::Music: return music_mute_;
    case Bus::SFX: return sfx_mute_;
    case Bus::Voice: return voice_mute_;
    case Bus::Ambient: return ambient_mute_;
    case Bus::UI: return ui_mute_;
    case Bus::Custom:
    default: return false;
    }
}

int AudioEngine::create_bus(const string &name)
{
    int id = next_custom_bus_id_++;
    BusState state;
    state.name = name;
    custom_buses_[id] = state;
    return id;
}

void AudioEngine::destroy_bus(int bus_id)
{
    custom_buses_.erase(bus_id);
}

void AudioEngine::add_bus_effect(Bus bus, shared_ptr<DSPNode> effect)
{
    get_bus_state(bus).effects.push_back(effect);
}

void AudioEngine::remove_bus_effect(Bus bus, shared_ptr<DSPNode> effect)
{
    auto &effects = get_bus_state(bus).effects;
    for (auto it = effects.begin(); it != effects.end(); ++it) {
        if (it->get() == effect.get()) {
            effects.erase(it);
            break;
        }
    }
}

void AudioEngine::set_bus_effect_param(Bus bus, int effect_index, const char *param, float value)
{
    auto &effects = get_bus_state(bus).effects;
    if (effect_index < 0 || static_cast<size_t>(effect_index) >= effects.size())
        return;
    effects[static_cast<size_t>(effect_index)]->set_param(param, value);
}

void AudioEngine::set_voice_count(int max_voices)
{
    max_voices_ = max_voices;
}

} // namespace audio
} // namespace q3d
