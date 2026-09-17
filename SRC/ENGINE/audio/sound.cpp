#include "sound.h"
#include "sound_manager.h"

namespace q3d {
namespace audio {

Sound::Sound(SoundID id, VoiceID voice)
    : id_(id), voice_id_(voice)
{
}

Sound::~Sound() = default;

void Sound::play()
{
    playing_ = true;
}

void Sound::play(float volume, float pitch)
{
    volume_ = volume;
    pitch_ = pitch;
    playing_ = true;
}

void Sound::stop()
{
    playing_ = false;
}

void Sound::pause()
{
    playing_ = false;
}

void Sound::resume()
{
    playing_ = true;
}

void Sound::seek(float /*seconds*/)
{
}

bool Sound::is_playing() const
{
    return playing_;
}

bool Sound::is_paused() const
{
    return false;
}

bool Sound::is_stopped() const
{
    return !playing_;
}

void Sound::set_volume(float v)
{
    volume_ = v;
}

void Sound::set_pitch(float p)
{
    pitch_ = p;
}

void Sound::set_position(const Vec3 &pos)
{
    pos_ = pos;
}

void Sound::set_loop(bool loop)
{
    loop_ = loop;
}

void Sound::set_on_end(function<void()> cb)
{
    on_end_ = cb;
}

void Sound::set_on_marker(const string &/*marker*/, function<void()> /*cb*/)
{
}

} // namespace audio
} // namespace q3d
