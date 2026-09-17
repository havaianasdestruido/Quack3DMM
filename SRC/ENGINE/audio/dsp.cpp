#include <cmath>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "dsp.h"

namespace q3d {
namespace audio {

string ReverbDSP::type() const
{
    return "reverb";
}

void ReverbDSP::process(float *samples, int count, int channels)
{
    (void)channels;
    for (int i = 0; i < count; ++i) {
        samples[i] = samples[i] * dry_ + samples[i] * wet_ * room_size_;
    }
}

void ReverbDSP::set_param(const char *name, float value)
{
    if (std::strcmp(name, "room_size") == 0) room_size_ = value;
    else if (std::strcmp(name, "damping") == 0) damping_ = value;
    else if (std::strcmp(name, "wet") == 0) wet_ = value;
    else if (std::strcmp(name, "dry") == 0) dry_ = value;
    else if (std::strcmp(name, "width") == 0) width_ = value;
}

float ReverbDSP::get_param(const char *name) const
{
    if (std::strcmp(name, "room_size") == 0) return room_size_;
    if (std::strcmp(name, "damping") == 0) return damping_;
    if (std::strcmp(name, "wet") == 0) return wet_;
    if (std::strcmp(name, "dry") == 0) return dry_;
    if (std::strcmp(name, "width") == 0) return width_;
    return 0.0f;
}

string DelayDSP::type() const
{
    return "delay";
}

void DelayDSP::process(float *samples, int count, int channels)
{
    (void)channels;
    for (int i = 0; i < count; ++i) {
        samples[i] += samples[i] * wet_ * feedback_;
    }
}

void DelayDSP::set_param(const char *name, float value)
{
    if (std::strcmp(name, "delay_time") == 0) delay_time_ = value;
    else if (std::strcmp(name, "feedback") == 0) feedback_ = value;
    else if (std::strcmp(name, "wet") == 0) wet_ = value;
}

float DelayDSP::get_param(const char *name) const
{
    if (std::strcmp(name, "delay_time") == 0) return delay_time_;
    if (std::strcmp(name, "feedback") == 0) return feedback_;
    if (std::strcmp(name, "wet") == 0) return wet_;
    return 0.0f;
}

string EQDSP::type() const
{
    return "eq";
}

void EQDSP::process(float *samples, int count, int channels)
{
    (void)channels;
    for (int i = 0; i < count; ++i) {
        samples[i] += low_gain_ + mid_gain_ + high_gain_;
    }
}

void EQDSP::set_param(const char *name, float value)
{
    if (std::strcmp(name, "low_gain") == 0) low_gain_ = value;
    else if (std::strcmp(name, "mid_gain") == 0) mid_gain_ = value;
    else if (std::strcmp(name, "high_gain") == 0) high_gain_ = value;
    else if (std::strcmp(name, "low_freq") == 0) low_freq_ = value;
    else if (std::strcmp(name, "high_freq") == 0) high_freq_ = value;
}

float EQDSP::get_param(const char *name) const
{
    if (std::strcmp(name, "low_gain") == 0) return low_gain_;
    if (std::strcmp(name, "mid_gain") == 0) return mid_gain_;
    if (std::strcmp(name, "high_gain") == 0) return high_gain_;
    if (std::strcmp(name, "low_freq") == 0) return low_freq_;
    if (std::strcmp(name, "high_freq") == 0) return high_freq_;
    return 0.0f;
}

string CompressorDSP::type() const
{
    return "compressor";
}

void CompressorDSP::process(float *samples, int count, int channels)
{
    (void)channels;
    const float threshold_linear = std::pow(10.0f, threshold_ / 20.0f);
    for (int i = 0; i < count; ++i) {
        float s = samples[i];
        float abs_s = std::fabs(s);
        if (abs_s > threshold_linear) {
            float over = abs_s - threshold_linear;
            over /= ratio_;
            float sign = s < 0.0f ? -1.0f : 1.0f;
            samples[i] = sign * (threshold_linear + over);
        }
    }
}

void CompressorDSP::set_param(const char *name, float value)
{
    if (std::strcmp(name, "threshold") == 0) threshold_ = value;
    else if (std::strcmp(name, "ratio") == 0) ratio_ = value;
    else if (std::strcmp(name, "attack") == 0) attack_ = value;
    else if (std::strcmp(name, "release") == 0) release_ = value;
}

float CompressorDSP::get_param(const char *name) const
{
    if (std::strcmp(name, "threshold") == 0) return threshold_;
    if (std::strcmp(name, "ratio") == 0) return ratio_;
    if (std::strcmp(name, "attack") == 0) return attack_;
    if (std::strcmp(name, "release") == 0) return release_;
    return 0.0f;
}

string DistortionDSP::type() const
{
    return "distortion";
}

void DistortionDSP::process(float *samples, int count, int channels)
{
    (void)channels;
    for (int i = 0; i < count; ++i) {
        float s = samples[i] * drive_;
        float distorted = std::tanh(s);
        samples[i] = samples[i] * (1.0f - mix_) + distorted * mix_;
    }
}

void DistortionDSP::set_param(const char *name, float value)
{
    if (std::strcmp(name, "drive") == 0) drive_ = value;
    else if (std::strcmp(name, "mix") == 0) mix_ = value;
}

float DistortionDSP::get_param(const char *name) const
{
    if (std::strcmp(name, "drive") == 0) return drive_;
    if (std::strcmp(name, "mix") == 0) return mix_;
    return 0.0f;
}

string ChorusDSP::type() const
{
    return "chorus";
}

void ChorusDSP::process(float *samples, int count, int channels)
{
    (void)channels;
    for (int i = 0; i < count; ++i) {
        samples[i] = samples[i] * (1.0f - mix_) + samples[i] * mix_;
    }
}

void ChorusDSP::set_param(const char *name, float value)
{
    if (std::strcmp(name, "rate") == 0) rate_ = value;
    else if (std::strcmp(name, "depth") == 0) depth_ = value;
    else if (std::strcmp(name, "mix") == 0) mix_ = value;
}

float ChorusDSP::get_param(const char *name) const
{
    if (std::strcmp(name, "rate") == 0) return rate_;
    if (std::strcmp(name, "depth") == 0) return depth_;
    if (std::strcmp(name, "mix") == 0) return mix_;
    return 0.0f;
}

string FlangerDSP::type() const
{
    return "flanger";
}

void FlangerDSP::process(float *samples, int count, int channels)
{
    (void)samples;
    (void)count;
    (void)channels;
}

void FlangerDSP::set_param(const char *name, float value)
{
    (void)name;
    (void)value;
}

float FlangerDSP::get_param(const char *name) const
{
    (void)name;
    return 0.0f;
}

string PitchShiftDSP::type() const
{
    return "pitch_shift";
}

void PitchShiftDSP::process(float *samples, int count, int channels)
{
    (void)samples;
    (void)count;
    (void)channels;
}

void PitchShiftDSP::set_param(const char *name, float value)
{
    if (std::strcmp(name, "semitones") == 0) semitones_ = value;
    else if (std::strcmp(name, "formants_preserve") == 0) formants_preserve_ = value;
}

float PitchShiftDSP::get_param(const char *name) const
{
    if (std::strcmp(name, "semitones") == 0) return semitones_;
    if (std::strcmp(name, "formants_preserve") == 0) return formants_preserve_;
    return 0.0f;
}

float ProceduralSound::sine(float phase)
{
    return std::sin(phase);
}

float ProceduralSound::square(float phase)
{
    return phase < static_cast<float>(M_PI) ? 1.0f : -1.0f;
}

float ProceduralSound::saw(float phase)
{
    return (2.0f / static_cast<float>(M_PI)) * (phase - static_cast<float>(M_PI));
}

float ProceduralSound::noise()
{
    static std::mt19937 rng{1234};
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(rng);
}

vector<float> ProceduralSound::tone(float freq, float duration, float sample_rate)
{
    int samples = static_cast<int>(duration * sample_rate);
    vector<float> out;
    out.reserve(static_cast<size_t>(samples));
    float phase = 0.0f;
    float step = 2.0f * static_cast<float>(M_PI) * freq / sample_rate;
    for (int i = 0; i < samples; ++i) {
        out.push_back(std::sin(phase));
        phase += step;
        if (phase > 2.0f * static_cast<float>(M_PI))
            phase -= 2.0f * static_cast<float>(M_PI);
    }
    return out;
}

vector<float> ProceduralSound::noise(float duration, float sample_rate)
{
    int samples = static_cast<int>(duration * sample_rate);
    vector<float> out;
    out.reserve(static_cast<size_t>(samples));
    for (int i = 0; i < samples; ++i)
        out.push_back(ProceduralSound::noise());
    return out;
}

vector<float> ProceduralSound::apply_envelope(
    const vector<float> &signal,
    float attack, float decay, float sustain, float release,
    float sample_rate)
{
    size_t n = signal.size();
    vector<float> out;
    out.reserve(n);

    int attack_samples = static_cast<int>(attack * sample_rate);
    int decay_samples = static_cast<int>(decay * sample_rate);
    int release_samples = static_cast<int>(release * sample_rate);
    int sustain_start = attack_samples + decay_samples;
    int sustain_end = static_cast<int>(n) - release_samples;
    if (sustain_end < sustain_start) sustain_end = sustain_start;

    for (size_t i = 0; i < n; ++i) {
        float env = 1.0f;
        int idx = static_cast<int>(i);
        if (idx < attack_samples) {
            env = static_cast<float>(idx) / static_cast<float>(attack_samples);
        } else if (idx < sustain_start) {
            float t = static_cast<float>(idx - attack_samples) / static_cast<float>(decay_samples);
            env = 1.0f + t * (sustain - 1.0f);
        } else if (idx < sustain_end) {
            env = sustain;
        } else {
            float t = static_cast<float>(idx - sustain_end) / static_cast<float>(release_samples);
            env = sustain * (1.0f - t);
        }
        out.push_back(signal[i] * env);
    }
    return out;
}

vector<float> ProceduralSound::fm(
    float carrier_freq, float mod_freq, float mod_index,
    float duration, float sample_rate)
{
    int samples = static_cast<int>(duration * sample_rate);
    vector<float> out;
    out.reserve(static_cast<size_t>(samples));
    float carrier_phase = 0.0f;
    float mod_phase = 0.0f;
    float carrier_step = 2.0f * static_cast<float>(M_PI) * carrier_freq / sample_rate;
    float mod_step = 2.0f * static_cast<float>(M_PI) * mod_freq / sample_rate;
    for (int i = 0; i < samples; ++i) {
        float mod_signal = std::sin(mod_phase) * mod_index;
        float sample = std::sin(carrier_phase + mod_signal);
        out.push_back(sample);
        carrier_phase += carrier_step;
        mod_phase += mod_step;
    }
    return out;
}

} // namespace audio
} // namespace q3d
