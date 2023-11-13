/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModuDistortAudioProcessorEditor::ModuDistortAudioProcessorEditor (ModuDistortAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    setSize (300, 500);

    distortionTypeButton.setButtonText("Distortion: Tube");
    distortionTypeButton.onClick = [this] { updateDistortionType(); };
    addAndMakeVisible(distortionTypeButton);

    setupSliderWithLabel(driveSlider, driveLabel, "Drive", juce::Slider::RotaryVerticalDrag);
    driveSlider.setRange(1.0, 50.0, 0.1);
    driveSlider.setValue(2.0);

    setupSliderWithLabel(mixSlider, mixLabel, "Mix", juce::Slider::RotaryVerticalDrag);
    mixSlider.setRange(0, 100, 1);
    mixSlider.setValue(100);

    setupSliderWithLabel(inputVolumeSlider, inputVolumeLabel, "Input Volume", juce::Slider::RotaryVerticalDrag);
    inputVolumeSlider.setRange(0.0, 2.0, 0.01);
    inputVolumeSlider.setValue(1.0);

    setupSliderWithLabel(outputVolumeSlider, outputVolumeLabel, "Output Volume", juce::Slider::RotaryVerticalDrag);
    outputVolumeSlider.setRange(0.0, 2.0, 0.01);
    outputVolumeSlider.setValue(1.0);

    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "drive", driveSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "mix", mixSlider);
    inputVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "inputVolume", inputVolumeSlider);
    outputVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "outputVolume", outputVolumeSlider);
}

ModuDistortAudioProcessorEditor::~ModuDistortAudioProcessorEditor()
{
}

//==============================================================================
void ModuDistortAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(30.0f);

    juce::Font modernFont("Sans-Sarif", 30.0f, juce::Font::bold);
    g.setFont(modernFont);

    juce::Rectangle<int> bounds = getLocalBounds();

    juce::Colour modernColor(0xEE96461E);
    
    g.setColour(modernColor);
    g.fillRect(bounds);

    g.setColour(juce::Colours::white);
    juce::Rectangle<float> roundedRect = bounds.toFloat().reduced(20.0f);
    g.drawRoundedRectangle(roundedRect, 15.0f, 2.0f);

    g.setColour(juce::Colours::white);
    g.drawText("Modu-DISTORT", bounds, juce::Justification::centred, true);

}

void ModuDistortAudioProcessorEditor::resized()
{
    int x = 60;
    int y = 55;
    int controlWidth = 80;
    int controlHeight = 60;
    int spacing = 20;

    distortionTypeButton.setBounds(50, 300, 200, 140);

    driveSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + spacing;

    mixSlider.setBounds(x, y, controlWidth, controlHeight);
    x = 60; 
    y += controlHeight + spacing + 20;

    inputVolumeSlider.setBounds(x, y, controlWidth, controlHeight);
    x += controlWidth + spacing;

    outputVolumeSlider.setBounds(x, y, controlWidth, controlHeight);
}

void ModuDistortAudioProcessorEditor::updateDistortionType()
{
    auto currentType = audioProcessor.getDistortionType();
    auto nextType = static_cast<ModuDistortAudioProcessor::DistortionType>((currentType + 1) % 4);
    audioProcessor.setDistortionType(nextType);

    switch (nextType) {
    case ModuDistortAudioProcessor::DistortionType::kTube:
        distortionTypeButton.setButtonText("Distortion: Tube");
        break;
    case ModuDistortAudioProcessor::DistortionType::hardClip:
        distortionTypeButton.setButtonText("Distortion: Hard Clip");
        break;
    case ModuDistortAudioProcessor::DistortionType::softClip:
        distortionTypeButton.setButtonText("Distortion: Soft Clip");
        break;
    case ModuDistortAudioProcessor::DistortionType::fuzz:
        distortionTypeButton.setButtonText("Distortion: Fuzz");
        break;

}
}

void ModuDistortAudioProcessorEditor::setupSliderWithLabel(juce::Slider& slider, juce::Label& label, const juce::String& labelText, juce::Slider::SliderStyle style)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.attachToComponent(&slider, false);

    int labelHeight = 20; 
    label.setBounds(slider.getX(), slider.getBottom() - 75, slider.getWidth(), 20);
    label.setJustificationType(juce::Justification::centred);
}
