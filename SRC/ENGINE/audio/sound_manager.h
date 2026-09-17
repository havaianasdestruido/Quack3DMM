#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "audio_engine.h"
#include "dsp.h"
#include "sound.h"

namespace q3d {
namespace audio {

using StrPtr = const char*;
using std::shared_ptr;
using std::string;
using std::vector;

struct PhonemeFrame {
    float viseme = 0.0f;
    float amplitude = 0.0f;
};

enum class AudioFormat {
    WAV,
    MP3,
    OGG,
    FLAC,
};

class SoundManager {
public:
    static SoundManager &instance();

    void load_bank(StrPtr vpath);
    void unload_bank(StrPtr vpath);
    void reload_bank(StrPtr vpath);

    void register_sound(SoundDefinition def);
    void unregister_sound(StrPtr id);
    const SoundDefinition *get_sound_def(StrPtr id) const;
    vector<string> find_sounds(const char *category) const;

    shared_ptr<Sound> play(StrPtr sound_id);
    shared_ptr<Sound> play(StrPtr sound_id, float volume, float pitch);
    shared_ptr<Sound> play_3d(StrPtr sound_id, const Vec3 &pos);

    void stop_all();
    void stop_category(const char *category);

    void bind_event(const char *event_name, StrPtr sound_id);
    void unbind_event(const char *event_name);
    void fire_event(const char *event_name);

    void set_music_layer(const char *layer_id, float volume);
    void crossfade_music(const char *from_id, const char *to_id, float fade_time);

    void start_capture(int sample_rate = 44100, int channels = 1);
    vector<float> read_capture(int max_samples);
    void stop_capture();
    bool is_capturing() const;

    vector<PhonemeFrame> generate_lipsync(const char *audio_vpath);

    void hot_reload_changed(const char *sound_vpath);

    void start_recording(const char *output_path, AudioFormat fmt);
    void stop_recording();

private:
    SoundManager();

    unordered_map<string, SoundDefinition> sound_defs_;
    unordered_map<string, string> event_bindings_;
    unordered_map<string, float> music_layers_;
    unordered_map<string, shared_ptr<Sound>> active_sounds_;

    int capture_sample_rate_ = 44100;
    int capture_channels_ = 1;
    bool capturing_ = false;
    vector<float> capture_buffer_;
};

} // namespace audio
} // namespace q3d