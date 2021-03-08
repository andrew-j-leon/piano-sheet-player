#include "stdafx.h"
#include "Measure.hpp"
#include <algorithm>

Measure::Measure()
{
}

void Measure::create(const std::string& trebleLine, const std::string& bassLine, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
	const unsigned long long& samplesPerWhole, unsigned long long& trebleSample, unsigned long long& bassSample)
{
	treble.create(trebleLine, noteTypeToSound, system, samplesPerWhole, trebleSample);
	bass.create(bassLine, noteTypeToSound, system, samplesPerWhole, bassSample);
}

bool Measure::finishedPlaying() const
{
	return (treble.finishedPlaying() && bass.finishedPlaying());
}

void Measure::setPause(bool paused)
{
	treble.setPause(paused);
	bass.setPause(paused);
}

Measure::~Measure()
{
}