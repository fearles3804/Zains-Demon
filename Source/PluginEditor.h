#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ZainsDemonAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ZainsDemonAudioProcessorEditor(ZainsDemonAudioProcessor&);
    ~ZainsDemonAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ZainsDemonAudioProcessor& processor;
    juce::Slider input, demon, pitch, drive, mix, output;
    juce::Label title, subtitle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> a1,a2,a3,a4,a5,a6;

    void setup(juce::Slider&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZainsDemonAudioProcessorEditor)
};
