/*
  ==============================================================================

    MySynthesiser.h
    Created: 27 Sep 2026 3:43:46pm
    Author:  yaega

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


using namespace juce;

class MySynthesiserSound : public SamplerSound
{
public:
    MySynthesiserSound(const juce::String& name,
        AudioFormatReader& source,
        const BigInteger& midiNotes,
        int* velocityRange,
        int midiNoteForNormalPitch,
        double attackTimeSecs,
        double releaseTimeSecs,
        double maxSampleLengthSeconds)
        : SamplerSound(name, source, midiNotes, midiNoteForNormalPitch, attackTimeSecs, releaseTimeSecs, maxSampleLengthSeconds),
        velocityRange(velocityRange)
    {}

	bool appliesToVelocity(int midiVelocity)
	{
		return midiVelocity >= velocityRange[0] && midiVelocity <= velocityRange[1];
	}

    int* getVelocityRange() { return velocityRange; }
private:
    int* velocityRange;
};

class MySynthesiser : public juce::Synthesiser
{
public:
    MySynthesiser()
        :Synthesiser()
    {};

    void noteOn(const int midiChannel,
        const int midiNoteNumber,
        const float velocity) override
    {
        const ScopedLock sl(lock);

        for (auto* sound : sounds)
        {
            if (sound->appliesToNote(midiNoteNumber) && sound->appliesToChannel(midiChannel) && 
                reinterpret_cast<MySynthesiserSound*>(sound)->appliesToVelocity(static_cast<int>(velocity * 127.0f)))
            {
                // If hitting a note that's still ringing, stop it first (it could be
                // still playing because of the sustain or sostenuto pedal).
                for (auto* voice : voices)
                    if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel(midiChannel))
                        stopVoice(voice, 1.0f, true);

                int* velocityRange = static_cast<MySynthesiserSound*>(sound)->getVelocityRange();
                float fixedVelocity = (velocity * 127.0f - velocityRange[0]) / (velocityRange[1] - velocityRange[0]);
                startVoice(findFreeVoice(sound, midiChannel, midiNoteNumber, shouldStealNotes),
                    sound, midiChannel, midiNoteNumber, fixedVelocity);
            }
        }
    }
private:
    bool shouldStealNotes = true;
};