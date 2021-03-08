#pragma once

#ifndef NOTE_HPP
#define NOTE_HPP

#include <fmod.h>
#include <fmod.hpp>

class Note
{
public:
	Note(double theNoteValue);
	void create(FMOD::System* theSystem, FMOD::Sound* theSound, bool paused,
		unsigned long long& startSample, const unsigned long long& samplesPerWhole);
	void setPaused(bool pauseValue);
	bool isPlaying() const;

private:
	double noteValue;
	FMOD::Channel* channel;
};

#endif