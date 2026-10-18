/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

static const juce::String vendorName = "Tokyo Toneworks";

//==============================================================================
/**
*/
class GuitarPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GuitarPluginAudioProcessor();
    ~GuitarPluginAudioProcessor() override;

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

	juce::Synthesiser* getSynth() { return synth; }
	juce::MidiKeyboardState* getKeyboardState() { return keyboardState; }
	juce::AudioParameterFloat* gainParameter;
private:
    //==============================================================================
    void setupSynth();
	void createSoundForArticulation(juce::Synthesiser* synth, juce::AudioFormatManager& manager, const std::unordered_map<juce::String, int*>& articulations, int minNoteNumber, int maxNoteNumber, int roundRobinCount);
    juce::Synthesiser* synth;
	juce::MidiKeyboardState* keyboardState;
    double currentSampleRate = 0.0;
    double currentGain = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarPluginAudioProcessor)
};
