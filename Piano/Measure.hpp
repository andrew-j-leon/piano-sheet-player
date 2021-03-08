#pragma once

#ifndef MEASURE_HPP
#define MEASURE_HPP

#include "HalfMeasure.hpp"

class Measure
{
public:
	Measure();
	void create(const std::string& trebleLine, const std::string& bassLine, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system, 
		const unsigned long long& samplesPerWhole, unsigned long long& trebleSample, unsigned long long& bassSample);
	bool finishedPlaying() const;
	void setPause(bool paused);
	~Measure();

private:
	HalfMeasure treble;
	HalfMeasure bass;
};


#endif