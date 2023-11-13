/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class ModuDistortAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    ModuDistortAudioProcessor();
    ~ModuDistortAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    enum DistortionType {
        kTube,
        hardClip,
        softClip,
        fuzz,
    };

    DistortionType distortionType = DistortionType::kTube;

    DistortionType getDistortionType() const;
    void setDistortionType(DistortionType newType);

    juce::AudioProcessorValueTreeState::ParameterLayout ModuDistortAudioProcessor::createParams();

    juce::AudioProcessorValueTreeState parameters;
    //static juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    juce::AudioProcessorValueTreeState& getParameters();
    

private:
    //==============================================================================

    
    
    //DistortionType distortionType = DistortionType::kTube;
    float drive = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModuDistortAudioProcessor)
};
