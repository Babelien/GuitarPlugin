/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MyKeyboard.h"

//==============================================================================
/**
*/
class GuitarPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::MidiInputCallback,
    public juce::Slider::Listener
{
public:
    GuitarPluginAudioProcessorEditor (GuitarPluginAudioProcessor&);
    ~GuitarPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // juce::MidiInputCallback overrides
	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;

	void sliderValueChanged(juce::Slider* slider) override;
private:

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarPluginAudioProcessor& audioProcessor;

    MyKeyboard keyboard;

    juce::Slider gainKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarPluginAudioProcessorEditor)
};
