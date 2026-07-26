#include "PluginProcessor.h"
#include "PluginEditor.h"

ZainsDemonAudioProcessor::ZainsDemonAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ZainsDemonAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "INPUT", "Input", -24.0f, 12.0f, 0.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DEMON", "Demon", 0.0f, 1.0f, 0.65f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "PITCH", "Pitch", -12.0f, 0.0f, -6.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DRIVE", "Drive", 0.0f, 1.0f, 0.35f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "MIX", "Mix", 0.0f, 1.0f, 1.0f));

    p.push_back(std::make_unique<juce::AudioParameterFloat>(
        "OUTPUT", "Output", -24.0f, 12.0f, 0.0f));

    return { p.begin(), p.end() };
}

void ZainsDemonAudioProcessor::prepareToPlay(double sr, int block)
{
    juce::dsp::ProcessSpec spec{
        sr,
        static_cast<juce::uint32>(block),
        static_cast<juce::uint32>(getTotalNumOutputChannels())
    };

    inputGain.prepare(spec);
    outputGain.prepare(spec);
    shaper.prepare(spec);
    lowpass.prepare(spec);
    dryWet.prepare(spec);

    pitchShifter.prepare(
        sr,
        block,
        getTotalNumOutputChannels()
    );

    inputGain.reset();
    outputGain.reset();
    shaper.reset();
    lowpass.reset();
    dryWet.reset();
}

bool ZainsDemonAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    auto in = layouts.getMainInputChannelSet();

    return in == juce::AudioChannelSet::mono()
        || in == juce::AudioChannelSet::stereo();
}

void ZainsDemonAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const float inputDb =
        apvts.getRawParameterValue("INPUT")->load();

    const float demon =
        apvts.getRawParameterValue("DEMON")->load();

    const float semitones =
        apvts.getRawParameterValue("PITCH")->load();

    const float drive =
        apvts.getRawParameterValue("DRIVE")->load();

    const float mix =
        apvts.getRawParameterValue("MIX")->load();

    const float outputDb =
        apvts.getRawParameterValue("OUTPUT")->load();

    inputGain.setGainDecibels(inputDb);
    outputGain.setGainDecibels(outputDb);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);

    inputGain.process(ctx);

    dryWet.setWetMixProportion(mix);
    dryWet.pushDrySamples(block);

    pitchShifter.setSemitones(semitones * demon);
    pitchShifter.process(buffer);

    const float amount = 1.0f + drive * 18.0f;

    // JUCE'nin Function tipine uygun şekilde std::function kullanıyoruz.
    shaper.functionToUse =
        [amount, demon](float x) -> float
    {
        const float saturated =
            std::tanh(x * amount);

        return x * (1.0f - demon)
             + saturated * demon;
    };

    shaper.process(ctx);

    const float cutoff =
        juce::jmap(
            demon,
            18000.0f,
            4200.0f
        );

    // IIR filtresinin state alanına doğrudan erişmek yerine
    // yeni katsayıları setCoefficients ile uyguluyoruz.
    auto coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            getSampleRate(),
            cutoff,
            0.707f
        );

    lowpass.coefficients = coefficients;

    lowpass.process(ctx);

    dryWet.mixWetSamples(block);

    outputGain.process(ctx);
}

void ZainsDemonAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
    {
        copyXmlToBinary(*xml, destData);
    }
}

void ZainsDemonAudioProcessor::setStateInformation(
    const void* data,
    int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        if (xml->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(
                juce::ValueTree::fromXml(*xml)
            );
        }
    }
}

juce::AudioProcessorEditor*
ZainsDemonAudioProcessor::createEditor()
{
    return new ZainsDemonAudioProcessorEditor(*this);
}
