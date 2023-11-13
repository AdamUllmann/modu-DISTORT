/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModuDistortAudioProcessor::ModuDistortAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    parameters(*this, nullptr, "Parameters", createParams())
#endif
{
    
}

ModuDistortAudioProcessor::~ModuDistortAudioProcessor()
{
}

//==============================================================================
const juce::String ModuDistortAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ModuDistortAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ModuDistortAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ModuDistortAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ModuDistortAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ModuDistortAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ModuDistortAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ModuDistortAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ModuDistortAudioProcessor::getProgramName (int index)
{
    return {};
}

void ModuDistortAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ModuDistortAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ModuDistortAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ModuDistortAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ModuDistortAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    auto drive = parameters.getRawParameterValue("drive")->load();
    auto mix = parameters.getRawParameterValue("mix")->load() / 100.0f; // Assuming mix is 0-100
    auto inputVolume = parameters.getRawParameterValue("inputVolume")->load();
    auto outputVolume = parameters.getRawParameterValue("outputVolume")->load();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float cleanSignal = channelData[sample] * inputVolume;

            // Processed (distorted) signal
            float processedSignal = cleanSignal; // Default to clean signal

            if (distortionType == DistortionType::kTube) {
                processedSignal = tanh(cleanSignal * drive);
            }
            if (distortionType == DistortionType::softClip) {
                if (cleanSignal > 0)
                    processedSignal = 1 - exp(-(cleanSignal * drive));
                else
                    processedSignal = -1 + exp(cleanSignal * drive);
            }
            if (distortionType == DistortionType::hardClip) {
                const float threshold = 0.5f;
                processedSignal = juce::jlimit(-threshold, threshold, (cleanSignal * drive));
            }
            if (distortionType == DistortionType::fuzz) {
                processedSignal = (cleanSignal) * (cleanSignal) * (cleanSignal) * drive;
            }
            channelData[sample] = (processedSignal * mix) + (cleanSignal * (1.0f - mix));
            channelData[sample] *= outputVolume;
        }
            // Add more distortion types here using if or switch statements
    }
}

//==============================================================================
bool ModuDistortAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ModuDistortAudioProcessor::createEditor()
{
    return new ModuDistortAudioProcessorEditor (*this);
}

//==============================================================================
void ModuDistortAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ModuDistortAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ModuDistortAudioProcessor();
}


ModuDistortAudioProcessor::DistortionType ModuDistortAudioProcessor::getDistortionType() const {
    return distortionType;
}

void ModuDistortAudioProcessor::setDistortionType(DistortionType newType) {
    distortionType = newType;

    // Optional: Add any additional logic needed when the distortion type changes
    // For example, resetting buffers or recalculating coefficients if necessary
}

juce::AudioProcessorValueTreeState::ParameterLayout ModuDistortAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", juce::NormalisableRange<float>(1.0f, 50.0f, 0.01), 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01), 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("inputVolume", "Input Volume", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("outputVolume", "Output Volume", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01), 1.0f));

    return { params.begin(), params.end() };
}

juce::AudioProcessorValueTreeState& ModuDistortAudioProcessor::getParameters() {
    return parameters;
}