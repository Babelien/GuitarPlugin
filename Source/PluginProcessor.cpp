/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <windows.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include "MySynthesiser.h"

//==============================================================================
GuitarPluginAudioProcessor::GuitarPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
	, keyboardState(new juce::MidiKeyboardState())
	, gainParameter(new juce::AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.5f))
{
    setupSynth();

	addParameter(gainParameter);
}

GuitarPluginAudioProcessor::~GuitarPluginAudioProcessor()
{
    delete synth;
    delete keyboardState;
}

//==============================================================================
const juce::String GuitarPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    currentSampleRate = sampleRate;
    synth->setCurrentPlaybackSampleRate(sampleRate);
}

void GuitarPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void GuitarPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    synth->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
	//keyboardState->processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), false);

    float maxMag = 24.0f;
	float gain = (*gainParameter - 0.5f) * maxMag + 1.0f;
	buffer.applyGain(juce::Decibels::decibelsToGain(gain));
}

//==============================================================================
bool GuitarPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarPluginAudioProcessor::createEditor()
{
    return new GuitarPluginAudioProcessorEditor (*this);
}

//==============================================================================
void GuitarPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

	juce::MemoryOutputStream(destData, true).writeFloat(*gainParameter);
}

void GuitarPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    *gainParameter = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readFloat();
}

void GuitarPluginAudioProcessor::createSoundForArticulation(
    juce::Synthesiser* synth, 
    juce::AudioFormatManager& manager, 
    const std::unordered_map<juce::String, int*>& articulations, 
    int minNoteNumber, 
    int maxNoteNumber, 
    int roundRobinCount)
{
    // SynthesiserSound
    for (const auto& articulation : articulations)
    {
        for (int noteNumber = minNoteNumber; noteNumber <= maxNoteNumber; ++noteNumber)
        {
            for (int roundRobin = 0; roundRobin < roundRobinCount; ++roundRobin)
            {
                juce::String sampleName(articulation.first + "_" + juce::String(noteNumber) + "_" + juce::String(roundRobin) + ".wav");

                juce::File file =
                    juce::File::getSpecialLocation(juce::File::commonDocumentsDirectory)
                    .getChildFile(vendorName)
                    .getChildFile(productName)
                    .getChildFile("Samples")
                    .getChildFile(articulation.first)
                    .getChildFile(sampleName);

                juce::AudioFormatReader* reader = manager.createReaderFor(file);
                if (reader == nullptr)
                {
                    DBG("FAILED: " + file.getFullPathName());
                    return;
                }
                //DBG(file.getFullPathName());

                juce::BigInteger noteRange;
                noteRange.setRange(noteNumber, 1, true);
                double attackTime = 0;
                double releaseTime = 0.01;
                double playTime = 10.0;
                juce::SamplerSound* sound = new MySynthesiserSound(sampleName, *reader, noteRange, articulation.second, noteNumber, attackTime, releaseTime, playTime);
                synth->addSound(sound);
                delete reader;
            }
        }
    }
}

void GuitarPluginAudioProcessor::setupSynth()
{
    synth = new MySynthesiser();

    juce::AudioFormatManager manager;
    manager.registerBasicFormats();

	// Articulations
    std::unordered_map<juce::String, int*> articulations;
    articulations["PalmMute"] = new int[2]{ 0, 39 };
    articulations["Sustain"] = new int[2]{ 40, 125 };
    articulations["PickingHarmonics"] = new int[2] { 126, 127 };

    // SynthesiserSound
	createSoundForArticulation(synth, manager, articulations, 40, 85, 1); // 1 Round Robin for Sustain articulation

	std::unordered_map<juce::String, int*> fx;
	fx["Brush"] = new int[2] { 0, 127 };

	createSoundForArticulation(synth, manager, fx, 93, 93, 1); // 1 Round Robin for Brush articulation

    // SynthesiserVoice
    for (int i = 0; i < 118; ++i) // 6 Strings Guitar
    {
        synth->addVoice(new juce::SamplerVoice());
    }

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarPluginAudioProcessor();
}
