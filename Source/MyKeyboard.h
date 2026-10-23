#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>

float const blackKeyHeightRatio = 0.69f;
float const blackKeyWidthRatio = 0.65f;

class Key : public juce::Component,
    public juce::Timer
{
public:
    Key(class MyKeyboard* keyboard, int noteNumber, int x, int y, int width, int height)
        :keyboard(keyboard), noteNumber(noteNumber)
    {
		setTopLeftPosition(x, y);
        setSize(width, height);
        startTimerHz(30);
    }

    int getNoteNumber()
    {
        return noteNumber;
    }

	void timerCallback() override
	{
        repaint();
	}

    static bool isWhiteKey(int noteNumber)
    {
        switch (noteNumber % 12)
        {
        case 0:
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
        case 11:
            return true;
            break;
        default:
            return false;
            break;
        }
    }

	void setDown(bool down)
	{
		isDown = down;
	}

    void mouseEnter(const juce::MouseEvent& event) override
    {
        onMouse = true;
    }

    void mouseExit(const juce::MouseEvent& event) override
    {
        onMouse = false;
    }

    void mouseDown(const juce::MouseEvent& event) override;

    void mouseUp(const juce::MouseEvent& event) override;

protected:
	class MyKeyboard* keyboard;
	juce::Colour keydownColour = juce::Colours::lightblue;
    int noteNumber;
	bool isDown = false;
    bool onMouse = false;
};

class WhiteKey : public Key
{
public:
	WhiteKey(class MyKeyboard* keyboard, int noteNumber, int x, int y, int width, int height)
		:Key(keyboard, noteNumber, x, y, width, height)
	{}

    void paint(juce::Graphics& g) override;
};

class BlackKey : public Key
{
public:
	BlackKey(class MyKeyboard* keyboard, int noteNumber, int x, int y, int width, int height)
		:Key(keyboard, noteNumber, x, y, width, height)
	{}

    void paint(juce::Graphics& g) override;
};

class MyKeyboard : public juce::Component,
    public juce::MidiKeyboardState::Listener
{
public:
    MyKeyboard(juce::MidiKeyboardState* state, juce::Synthesiser* synthesiser);
    virtual ~MyKeyboard();

    void paint(juce::Graphics& g) override;
    void resized() override;
	void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
	void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

    void setKeyWidth(int width)
    {
        keyWidth = width;
		blackKeyWidth = keyWidth * blackKeyWidthRatio;
    }

	void setAvailableRange(int low, int high)
	{
        lowestNote = Key::isWhiteKey(low) ? low : low - 1;
        highestNote = Key::isWhiteKey(high) ? high : high + 1;
	}

    int getKeyCoordRaw(int noteNumber)
    {
        int octaveX = 7 * keyWidth * (noteNumber / 12);
        if (Key::isWhiteKey(noteNumber))
        {
            return keyWidth * ceil((noteNumber % 12) / 2.0f) + octaveX;
        }
        else // Black key
        {
            int gap = blackKeyWidth * 0.9f;
            if (noteNumber % 12 <= 3) // C# or D#
            {
                return gap * (noteNumber % 12 - 2) + static_cast<int>(keyWidth * 3.0f / 2.0f - blackKeyWidth / 2.0f) + octaveX;
            }
            else
            {
                return gap * (noteNumber % 12 - 8) + keyWidth * 5 - static_cast<int>( blackKeyWidth / 2.0f) + octaveX;
            }
        }
    }

    int getKeyCoord(int noteNumber)
    {
        return getKeyCoordRaw(noteNumber) - getKeyCoordRaw(lowestNote);
    }

    int getHighestNote() const
	{
		return highestNote;
	}

    void createKeys()
    {
        for (int i = lowestNote; i <= highestNote; ++i)
        {
            Key* key;

            if (Key::isWhiteKey(i))
            {
                int x = getKeyCoord(i);
                int y = 0;
                int width = keyWidth;
                int height = getHeight();
				key = new WhiteKey(this, i, x, y, width, height);
                whiteKeys.emplace_back(key);
                addAndMakeVisible(key);
            }
            else
            {
                int x = getKeyCoord(i);
                int y = 0;
                int width = blackKeyWidth;
                int height = getHeight() * blackKeyHeightRatio;
				key = new BlackKey(this, i, x, y, width, height);
                blackKeys.emplace_back(key);
            }

        }

        for(const auto& key : blackKeys)
        {
            addAndMakeVisible(key);
        }
    }

    void updateKeys()
    {
        for (auto& key : whiteKeys)
        {
            key->setSize(keyWidth, getHeight());
        }
        for (auto& key : blackKeys)
        {
            key->setSize(blackKeyWidth, getHeight() * blackKeyHeightRatio);
        }
    }

	juce::Synthesiser* getSynth() const
	{
		return synth;
	}

private:
    juce::Synthesiser* synth;
    std::vector<Key*> whiteKeys;
    std::vector<Key*> blackKeys;
    std::vector<Key*> downKeys;
    juce::MidiKeyboardState* keyboardState;
    int keyWidth;
    int blackKeyWidth;
    int lowestNote, highestNote;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyKeyboard)
};