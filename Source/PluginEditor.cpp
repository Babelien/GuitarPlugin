#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarPluginAudioProcessorEditor::GuitarPluginAudioProcessorEditor(GuitarPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    keyboard(audioProcessor.getKeyboardState(), audioProcessor.getSynth())
{
	const int windowWidth = 1000, windowHeight = 600;
    setSize(windowWidth, windowHeight);
	auto area = getLocalBounds();

    area.removeFromBottom(windowHeight * 0.18f);

    // Gain Knob
	int knobSize = 200;
    gainKnob.setSliderStyle(juce::Slider::Rotary);                 
    gainKnob.setBounds(windowWidth / 2 - knobSize / 2, area.getHeight() / 2 - knobSize / 2, knobSize, knobSize);
    gainKnob.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);    
    gainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); 
    gainKnob.setRange(0.0, 1.0, 0.01);                              
    gainKnob.setValue(*audioProcessor.gainParameter);
	gainKnob.setDoubleClickReturnValue(true, 0.5);
    gainKnob.addListener(this);                      
    addAndMakeVisible(gainKnob);

    // Keyboard
	addAndMakeVisible(keyboard);
    keyboard.setAvailableRange(24, 108);
    keyboard.setKeyWidth(20);
    keyboard.createKeys();
}

GuitarPluginAudioProcessorEditor::~GuitarPluginAudioProcessorEditor()
{
}

//==============================================================================
void GuitarPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("Demonic Guitar", getLocalBounds(), juce::Justification::centred, 1);


}

void GuitarPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();

	keyboard.setBounds(area.removeFromBottom(getHeight() * 0.18));
}

void GuitarPluginAudioProcessorEditor::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
	audioProcessor.getKeyboardState()->processNextMidiEvent(message);
}


void GuitarPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &gainKnob)
    {
        *audioProcessor.gainParameter = slider->getValue();
    }
}
