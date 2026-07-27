#pragma once

#include <JuceHeader.h>
#include <cmath>

class SimplePitchShifter
{
public:
    void prepare(double sr, int maxBlock, int channels)
    {
        juce::ignoreUnused(maxBlock);

        sampleRate = sr;
        numChannels = juce::jmax(1, channels);

        bufferSize = juce::nextPowerOfTwo(
            static_cast<int>(sampleRate * 0.20)
        );

        buffer.setSize(numChannels, bufferSize);
        buffer.clear();

        writePosition = 0;
        readPosition = 0.0;
    }

    void reset()
    {
        buffer.clear();
        writePosition = 0;
        readPosition = 0.0;
    }

    void setSemitones(float st)
    {
        st = juce::jlimit(-12.0f, 0.0f, st);

        pitchRatio = std::pow(
            2.0f,
            st / 12.0f
        );
    }

    void process(juce::AudioBuffer<float>& audio)
    {
        const int numSamples = audio.getNumSamples();

        if (numSamples <= 0 || bufferSize <= 0)
            return;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            for (int channel = 0;
                 channel < numChannels;
                 ++channel)
            {
                const int sourceChannel =
                    juce::jmin(
                        channel,
                        audio.getNumChannels() - 1
                    );

                buffer.setSample(
                    channel,
                    writePosition,
                    audio.getSample(
                        sourceChannel,
                        sample
                    )
                );
            }

            for (int channel = 0;
                 channel < audio.getNumChannels();
                 ++channel)
            {
                const int bufferChannel =
                    juce::jmin(
                        channel,
                        numChannels - 1
                    );

                double readPos =
                    static_cast<double>(writePosition)
                    - 4096.0
                    + readPosition;

                while (readPos < 0.0)
                    readPos += bufferSize;

                while (readPos >= bufferSize)
                    readPos -= bufferSize;

                const int index0 =
                    static_cast<int>(readPos);

                const int index1 =
                    (index0 + 1)
                    & (bufferSize - 1);

                const float fraction =
                    static_cast<float>(
                        readPos - index0
                    );

                const float s0 =
                    buffer.getSample(
                        bufferChannel,
                        index0
                    );

                const float s1 =
                    buffer.getSample(
                        bufferChannel,
                        index1
                    );

                const float output =
                    s0 + (s1 - s0) * fraction;

                audio.setSample(
                    channel,
                    sample,
                    output
                );
            }

            writePosition =
                (writePosition + 1)
                & (bufferSize - 1);

            readPosition += pitchRatio - 1.0;

            if (readPosition > 1.0)
                readPosition -= 1.0;

            if (readPosition < -1.0)
                readPosition += 1.0;
        }
    }

private:
    juce::AudioBuffer<float> buffer;

    double sampleRate = 44100.0;

    int numChannels = 2;
    int bufferSize = 16384;

    int writePosition = 0;

    double readPosition = 0.0;

    float pitchRatio = 1.0f;
};
