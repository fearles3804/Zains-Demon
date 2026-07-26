#include "PluginEditor.h"

ZainsDemonAudioProcessorEditor::ZainsDemonAudioProcessorEditor(ZainsDemonAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(720, 420);

    title.setText("ZAIN'S DEMON", juce::dontSendNotification);
    title.setFont(juce::Font(32.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(title);

    subtitle.setText("DARK VOCAL FX  //  V0.2", juce::dontSendNotification);
    subtitle.setFont(juce::Font(13.0f));
    subtitle.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(subtitle);

    setup(input, "INPUT"); setup(demon, "DEMON"); setup(pitch, "PITCH");
    setup(drive, "DRIVE"); setup(mix, "MIX"); setup(output, "OUTPUT");

    a1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "INPUT", input);
    a2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "DEMON", demon);
    a3 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "PITCH", pitch);
    a4 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "DRIVE", drive);
    a5 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "MIX", mix);
    a6 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "OUTPUT", output);
}

void ZainsDemonAudioProcessorEditor::setup(juce::Slider& s, const juce::String&)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    s.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(s);
}

void ZainsDemonAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff090909));
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(18.0f), 16.0f);
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(18.0f), 16.0f, 1.0f);
}

void ZainsDemonAudioProcessorEditor::resized()
{
    title.setBounds(40, 28, 400, 42);
    subtitle.setBounds(43, 68, 300, 20);

    const int y = 145, w = 105, gap = 10, x0 = 42;
    juce::Slider* s[] = { &input, &demon, &pitch, &drive, &mix, &output };
    for (int i = 0; i < 6; ++i)
        s[i]->setBounds(x0 + i * (w + gap), y, w, 180);
}
