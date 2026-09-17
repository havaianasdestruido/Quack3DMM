#include "audio/dsp.h"
#include "audio/sound.h"

#include <cassert>
#include <cmath>
#include <vector>

int main()
{
    q3d::audio::SoundDefinition def;
    def.id = "tone";
    def.path = "vfs:/tone.wav";
    def.category = "test";

    auto tone = q3d::audio::ProceduralSound::tone(440.0f, 1.0f, 44100.0f);
    assert(tone.size() == 44100u);
    assert(std::fabs(tone[0]) < 1e-6f);
    assert(std::fabs(tone[11025] - 1.0f) < 0.02f);

    q3d::audio::ReverbDSP reverb;
    std::vector<float> wet = tone;
    reverb.set_param("wet", 0.5f);
    reverb.process(wet.data(), static_cast<int>(wet.size()), 1);
    assert(std::fabs(wet[11025]) > std::fabs(tone[11025]) * 0.5f);
    return 0;
}