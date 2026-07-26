#pragma once
#include <JuceHeader.h>

// Lightweight original pitch-shifting prototype.
// Two moving read heads are crossfaded through a circular delay buffer.
class SimplePitchShifter
{
public:
    void prepare(double sr, int maxBlock, int channels)
    {
        sampleRate = sr;
        numChannels = juce::jmax(1, channels);
        bufferSize = juce::nextPowerOfTwo((int)(sampleRate * 0.25));
        buffer.setSize(numChannels, bufferSize);
        buffer.clear();
        writePos = 0;
        phase = 0.0;
    }

    void reset()
    {
        buffer.clear();
        writePos = 0;
        phase = 0.0;
    }

    void setSemitones(float st)
    {
        ratio = std::pow(2.0f, st / 12.0f);
    }

    void process(juce::AudioBuffer<float>& audio)
    {
        const int n = audio.getNumSamples();
        const float delay = (float)juce::jlimit(64.0, (double)bufferSize - 64.0,
                                                sampleRate * 0.055);
        const double increment = (double)ratio - 1.0;

        for (int i = 0; i < n; ++i)
        {
            const float window = 0.5f - 0.5f * std::cos(2.0f * juce::MathConstants<float>::pi * (float)phase);
            const float wA = std::cos(window * juce::MathConstants<float>::halfPi);
            const float wB = std::sin(window * juce::MathConstants<float>::halfPi);

            for (int ch = 0; ch < numChannels; ++ch)
            {
                const float in = audio.getSample(ch, i);
                buffer.setSample(ch, writePos, in);

                auto readAt = [&](float offset) {
                    float pos = (float)writePos - offset;
                    while (pos < 0.0f) pos += (float)bufferSize;
                    const int p0 = (int)pos;
                    const int p1 = (p0 + 1) & (bufferSize - 1);
                    const float frac = pos - (float)p0;
                    return buffer.getSample(ch, p0) * (1.0f - frac)
                         + buffer.getSample(ch, p1) * frac;
                };

                float headA = readAt(delay * (1.0f - (float)phase));
                float headB = readAt(delay * (float)phase);
                audio.setSample(ch, i, headA * wA + headB * wB);
            }

            writePos = (writePos + 1) & (bufferSize - 1);
            phase += increment * 0.0035;
            while (phase >= 1.0) phase -= 1.0;
            while (phase < 0.0) phase += 1.0;
        }
    }

private:
    juce::AudioBuffer<float> buffer;
    double sampleRate = 44100.0;
    int numChannels = 2;
    int bufferSize = 16384;
    int writePos = 0;
    double phase = 0.0;
    float ratio = 1.0f;
};
