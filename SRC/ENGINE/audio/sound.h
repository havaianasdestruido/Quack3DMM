#pragma once

#include <functional>
#include <memory>
#include <string>

#include "audio_engine.h"

namespace q3d {
namespace audio {

using std::function;
using std::shared_ptr;
using std::string;

using SoundID = long;
using VoiceID = long;

struct SoundDefinition {
    string id;
    string path;
    float volume = 1.0f;
    float pitch = 1.0f;
    float falloff_start = 10.0f;
    float falloff_end = 100.0f;
    bool loop = false;
    Bus bus = Bus::SFX;
    int max_instances = 8;
    string category;
    bool is_music = false;
    float fade_in = 0.0f;
    float fade_out = 0.0f;
    int loop_count = -1;
};

class Sound {
public:
    ~Sound();

    SoundID id() const { return id_; }

    void play();
    void play(float volume, float pitch);
    void stop();
    void pause();
    void resume();
    void seek(float seconds);

    bool is_playing() const;
    bool is_paused() const;
    bool is_stopped() const;

    void set_volume(float v);
    void set_pitch(float p);
    void set_position(const Vec3 &pos);
    void set_loop(bool loop);

    void set_on_end(function<void()> cb);
    void set_on_marker(const string &marker, function<void()> cb);

    VoiceID voice_id() const { return voice_id_; }

private:
    bool playing_ = false;

public:
    friend class SoundManager;
    Sound(SoundID id, VoiceID voice);

    SoundID id_;
    VoiceID voice_id_;
    bool owned_ = false;

    float volume_ = 1.0f;
    float pitch_ = 1.0f;
    Vec3 pos_{};
    bool loop_ = false;

    function<void()> on_end_;
};

} // namespace audio
} // namespace q3d