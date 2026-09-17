#include "sound_manager.h"

namespace q3d {
namespace audio {

SoundManager &SoundManager::instance()
{
    static SoundManager inst;
    return inst;
}

SoundManager::SoundManager() = default;

void SoundManager::load_bank(StrPtr /*vpath*/)
{
}

void SoundManager::unload_bank(StrPtr /*vpath*/)
{
}

void SoundManager::reload_bank(StrPtr /*vpath*/)
{
}

void SoundManager::register_sound(SoundDefinition def)
{
    sound_defs_[def.id] = def;
}

void SoundManager::unregister_sound(StrPtr id)
{
    sound_defs_.erase(id);
}

const SoundDefinition *SoundManager::get_sound_def(StrPtr id) const
{
    auto it = sound_defs_.find(id);
    if (it != sound_defs_.end())
        return &it->second;
    return nullptr;
}

vector<string> SoundManager::find_sounds(const char *category) const
{
    vector<string> result;
    for (const auto &kv : sound_defs_) {
        if (kv.second.category == category)
            result.push_back(kv.first);
    }
    return result;
}

shared_ptr<Sound> SoundManager::play(StrPtr sound_id)
{
    auto it = sound_defs_.find(sound_id);
    if (it == sound_defs_.end())
        return nullptr;

    static long next_sound_id = 1;
    static long next_voice_id = 1;
    auto sound = std::make_shared<Sound>(next_sound_id++, next_voice_id++);
    sound->owned_ = true;
    active_sounds_[std::to_string(sound->id())] = sound;
    return sound;
}

shared_ptr<Sound> SoundManager::play(StrPtr sound_id, float volume, float pitch)
{
    auto sound = play(sound_id);
    if (sound)
        sound->play(volume, pitch);
    return sound;
}

shared_ptr<Sound> SoundManager::play_3d(StrPtr sound_id, const Vec3 &pos)
{
    auto sound = play(sound_id);
    if (sound)
        sound->set_position(pos);
    return sound;
}

void SoundManager::stop_all()
{
    active_sounds_.clear();
}

void SoundManager::stop_category(const char *category)
{
    for (auto it = active_sounds_.begin(); it != active_sounds_.end(); ) {
        auto def = get_sound_def(it->first.c_str());
        if (def && def->category == category)
            it = active_sounds_.erase(it);
        else
            ++it;
    }
}

void SoundManager::bind_event(const char *event_name, StrPtr sound_id)
{
    event_bindings_[event_name] = sound_id;
}

void SoundManager::unbind_event(const char *event_name)
{
    event_bindings_.erase(event_name);
}

void SoundManager::fire_event(const char *event_name)
{
    auto it = event_bindings_.find(event_name);
    if (it != event_bindings_.end())
        play(it->second.c_str());
}

void SoundManager::set_music_layer(const char *layer_id, float volume)
{
    music_layers_[layer_id] = volume;
}

void SoundManager::crossfade_music(const char *from_id, const char *to_id, float fade_time)
{
    (void)from_id;
    (void)to_id;
    (void)fade_time;
}

void SoundManager::start_capture(int sample_rate, int channels)
{
    capture_sample_rate_ = sample_rate;
    capture_channels_ = channels;
    capturing_ = true;
    capture_buffer_.clear();
}

vector<float> SoundManager::read_capture(int max_samples)
{
    if (!capturing_)
        return {};

    size_t to_read = static_cast<size_t>(std::min(max_samples, 1024));
    vector<float> result(to_read, 0.0f);
    capture_buffer_.insert(capture_buffer_.end(), result.begin(), result.end());
    return result;
}

void SoundManager::stop_capture()
{
    capturing_ = false;
}

bool SoundManager::is_capturing() const
{
    return capturing_;
}

vector<PhonemeFrame> SoundManager::generate_lipsync(const char *audio_vpath)
{
    (void)audio_vpath;
    return {};
}

void SoundManager::hot_reload_changed(const char *sound_vpath)
{
    (void)sound_vpath;
}

void SoundManager::start_recording(const char *output_path, AudioFormat fmt)
{
    (void)output_path;
    (void)fmt;
}

void SoundManager::stop_recording()
{
}

} // namespace audio
} // namespace q3d
