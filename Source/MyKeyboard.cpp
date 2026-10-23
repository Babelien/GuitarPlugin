/*
  ==============================================================================

    MyKeyboard.cpp
    Created: 19 Oct 2026 7:00:36pm
    Author:  yaega

  ==============================================================================
*/

#include "MyKeyboard.h"
#include "PluginProcessor.h"


MyKeyboard::MyKeyboard(juce::MidiKeyboardState* state, juce::Synthesiser* synthesiser)
	:synth(synthesiser)
	, keyboardState(state)
{
	keyboardState->addListener(this);
}

MyKeyboard::~MyKeyboard()
{
	keyboardState->removeListener(this);

	for (auto& key : whiteKeys)
	{
		delete key;
	}
	for (auto& key : blackKeys)
	{
		delete key;
	}
}

void MyKeyboard::paint(juce::Graphics& g)
{

}

void MyKeyboard::resized()
{
    Component::resized();

    updateKeys();
}

void MyKeyboard::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
	
	if (Key::isWhiteKey(midiNoteNumber))
	{
		for (auto& key : whiteKeys)
		{
			if (key->getNoteNumber() == midiNoteNumber)
			{
				key->setDown(true);
				break;
			}
		}
	}
	else
	{
		for (auto& key : blackKeys)
		{
			if (key->getNoteNumber() == midiNoteNumber)
			{
				key->setDown(true);
				break;
			}
		}
	}
	
}

void MyKeyboard::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
	
	if (Key::isWhiteKey(midiNoteNumber))
	{
		for (auto& key : whiteKeys)
		{
			if (key->getNoteNumber() == midiNoteNumber)
			{
				key->setDown(false);
				break;
			}
		}
	}
	else
	{
		for (auto& key : blackKeys)
		{
			if (key->getNoteNumber() == midiNoteNumber)
			{
				key->setDown(false);
				break;
			}
		}
	}
	
}

void WhiteKey::paint(juce::Graphics& g)
{
	g.setOpacity(1.0f);
	g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
	g.fillAll(isDown || onMouse? keydownColour : juce::Colours::white);
	g.setColour(juce::Colours::grey);
	g.drawLine(0.0f, 0.0f, 0.0f, static_cast<float>(getHeight()), 2.0f);

	if (noteNumber == keyboard->getHighestNote())
	{
		g.drawLine(static_cast<float>(getWidth()) - 1.0f, 0.0f, static_cast<float>(getWidth()) - 1.0f, static_cast<float>(getHeight()), 1.0f);
	}
}

void BlackKey::paint(juce::Graphics& g)
{
	g.setOpacity(1.0f);
	g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
	g.fillAll(isDown || onMouse ? keydownColour : juce::Colours::black);
}

void Key::mouseDown(const juce::MouseEvent& event)
{
	if (event.mods.isLeftButtonDown())
	{
		float velocity = event.getMouseDownY() / static_cast<float>(getHeight());
		keyboard->getSynth()->noteOn(1, noteNumber, velocity);
	}
}

void Key::mouseUp(const juce::MouseEvent& event)
{
	keyboard->getSynth()->noteOff(1, noteNumber, 0.8f, false);
}
