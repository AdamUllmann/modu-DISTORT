/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, 6.0f); // Rounded corners
    }
};

//==============================================================================
/**
*/
class ModuDistortAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ModuDistortAudioProcessorEditor (ModuDistortAudioProcessor&);
    ~ModuDistortAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void updateDistortionType();
    void setupSliderWithLabel(juce::Slider& slider, juce::Label& label, const juce::String& labelText, juce::Slider::SliderStyle style);

    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    //juce::AudioProcessorValueTreeState parameters;

    ModuDistortAudioProcessor& audioProcessor;

    juce::TextButton distortionTypeButton;
    juce::Slider driveSlider, mixSlider, inputVolumeSlider, outputVolumeSlider;
    juce::Label driveLabel, mixLabel, inputVolumeLabel, outputVolumeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputVolumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputVolumeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModuDistortAudioProcessorEditor)
};
