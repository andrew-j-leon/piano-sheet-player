#include "stdafx.h"
#include "Note.hpp"


Note::Note(double theNoteValue)
	: noteValue(theNoteValue)
{
	channel = nullptr;
}

void Note::create(FMOD::System* theSystem, FMOD::Sound* theSound, bool paused,
	unsigned long long& startSample, const unsigned long long& samplesPerWhole)
{
	theSystem->playSound(theSound, nullptr, paused, &channel);

	unsigned long long stopSample = startSample + (noteValue * samplesPerWhole);

	if (noteValue <= (1/8.0))
		channel->setDelay(startSample, stopSample + ((noteValue / 2.0) * samplesPerWhole), true);

	else
		channel->setDelay(startSample, stopSample + ((1/16.0) * samplesPerWhole), true);

	startSample = stopSample;
}

void Note::setPaused(bool pauseValue)
{
	channel->setPaused(pauseValue);
}

bool Note::isPlaying() const
{
	bool result;
	channel->isPlaying(&result);
	return result;
}