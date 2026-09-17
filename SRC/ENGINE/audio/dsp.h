#pragma once

#include <string>
#include <vector>

namespace q3d {
namespace audio {

using std::string;
using std::vector;

class DSPNode {
public:
    virtual ~DSPNode() = default;
    virtual string type() const = 0;
    virtual void process(float *samples, int count, int channels) = 0;
    virtual void set_param(const char *name, float value) = 0;
    virtual float get_param(const char *name) const = 0;
    virtual void reset() {}
};

class ReverbDSP : public DSPNode {
public:
    string type() const override;
    void process(float *samples, int count, int channels) override;
    void set_param(const char *name, float value) override;
    float get_param(const char *name) const override;

private:
    float room_size_ = 0.5f;
    float damping_ = 0.5f;
    float wet_ = 0.3f;
    float dry_ = 0.7f;
    float width_ = 1.0f;
};

class DelayDSP : public DSPNode {
public:
    string type() const override;
    void process(float *samples, int count, int channels) override;
    void set_param(const char *name, float value) override;
    float get_param(const char *name) const override;

private:
    float delay_time_ = 0.3f;
    float feedback_ = 0.3f;
    float wet_ = 0.5f;
};

class EQDSP : public DSPNode {
public:
    string type() const override;
    void process(float *samples, int count, int channels) override;
    void set_param(const char *name, float value) override;
    float get_param(const char *name) const override;

private:
    float low_gain_ = 0.0f;
    float mid_gain_ = 0.0f;
    float high_gain_ = 0.0f;
    float low_freq_ = 200.0f;
    float high_freq_ = 5000.0f;
};

class CompressorDSP : public DSPNode {
public:
    string type() const override;
    void process(float *samples, int count, int channels) override;
    void set_param(const char *name, float value) override;
    float get_param(const char *name) const override;

private:
    float threshold_ = -12.0f;
    float ratio_ = 2.0f;
    float attack_ = 0.01f;
    float release_ = 0.1f;
};

class DistortionDSP : public DSPNode {
public:
    string type() const override;
    void process(float *samples, int count, int channels) override;
    void set_param(const char *name, float value) override;
    float get_param(const char *name) const override;

private:
    float drive_ = 0.5f;
    float mix_ = 0.5f;
};

class ChorusDSP : public DSPNode {
public:
    string type() const override;
    void process(float *samples, int count, int channels) override;
    void set_param(const char *name, float value) override;
    float get_param(const char *name) const override;

private:
    float rate_ = 1.5f;
    float depth_ = 0.02f;
    float mix_ = 0.5f;
};

class FlangerDSP : public DSPNode {
public:
    string type() const override;
    void process(float *samples, int count, int channels) override;
    void set_param(const char *name, float value) override;
    float get_param(const char *name) const override;
};

class PitchShiftDSP : public DSPNode {
public:
    string type() const override;
    void process(float *samples, int count, int channels) override;
    void set_param(const char *name, float value) override;
    float get_param(const char *name) const override;

private:
    float semitones_ = 0.0f;
    float formants_preserve_ = 0.0f;
};

class ProceduralSound {
public:
    static float sine(float phase);
    static float square(float phase);
    static float saw(float phase);
    static float noise();

    static vector<float> tone(float freq, float duration, float sample_rate = 44100.0f);
    static vector<float> noise(float duration, float sample_rate = 44100.0f);

    static vector<float> apply_envelope(
        const vector<float> &signal,
        float attack, float decay, float sustain, float release,
        float sample_rate = 44100.0f);

    static vector<float> fm(
        float carrier_freq, float mod_freq, float mod_index,
        float duration, float sample_rate = 44100.0f);
};

} // namespace audio
} // namespace q3d