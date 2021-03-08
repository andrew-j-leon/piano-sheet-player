#pragma once

#ifndef PIANOSONG_HPP
#define PIANOSONG_HPP

#include "Measure.hpp"
#include <vector>
#include <string>
#include <queue>

class PianoSong
{
public:
	PianoSong();
	void play(const char* filename);
	~PianoSong();

private:

	FMOD_RESULT result;
	FMOD::System* system;

	FMOD::Sound* sounds[88];
	std::unordered_map<std::string, FMOD::Sound*> noteTypeToSound;
	std::queue<Measure> queue;

	int sampleRate;
	unsigned long long samplesPerWhole, trebleSample, bassSample;

	bool shouldAddToQueue();
	void addToQueue(std::istream& infile);

	void readBPM(std::istream& infile);
	void readSignature(std::istream& infile);
	void calculateSamplesPerWhole(double beatsPerMin, double beatsPerWhole);

	void createNoteTypeToSound(const std::vector<std::string>& noteTypeVector);
	void createSounds(const std::vector<std::string>& filenameVector);



	static std::vector<std::string> filenameVectorCreator();
	static std::vector<std::string> noteTypeArrayCreator();

	static bool isACommentLine(const std::string& line);
	static bool shouldResetKeyNumber(int index);

	static const char* getKeyLetter(int index);
	static const char* keyNumberToCharStr(int keyNumber);

	static std::string makeNoteType(const char* keyLetter, int keyNumber);
	static std::string makeWavFilename(const char* keyLetter, int keyNumber);

	static void resetKeyNumber(int& keyNumber, int index);
};


#endif