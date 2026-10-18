/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GuitarPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
	public juce::MidiKeyboardStateListener,
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

	void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
	void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

	void sliderValueChanged(juce::Slider* slider) override;
private:

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarPluginAudioProcessor& audioProcessor;

	juce::MidiKeyboardComponent keyboardComponent;

    juce::Slider gainKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarPluginAudioProcessorEditor)
};
