#include "stdafx.h"
#include "PianoSong.hpp"
#include <fstream>
#include <thread>
#include <functional>
#include "FMODErrorCheck.hpp"

PianoSong::PianoSong()
	: trebleSample(0), bassSample(0)
{
	result = FMOD::System_Create(&system);
	ERRCHECK(result);
	result = system->init(4000, FMOD_INIT_NORMAL, 0);
	ERRCHECK(result);
	result = system->getSoftwareFormat(&sampleRate, 0, 0);
	ERRCHECK(result);

	std::vector<std::string> noteTypeVector(std::move(noteTypeArrayCreator()));
	std::vector<std::string> filenameVector(std::move(filenameVectorCreator()));

	createSounds(filenameVector);
	createNoteTypeToSound(noteTypeVector);
}

void PianoSong::play(const char* filename)
{
	std::ifstream infile(filename);

	readBPM(infile);
	readSignature(infile);

	std::string treble, bass;
	bool finished(false);
	while (!finished)
	{
		if (queue.size() >= 1 && queue.front().finishedPlaying())
			queue.pop();

		while (!infile.eof())
		{
			addToQueue(infile);
			if (infile.eof())
				infile.close();
		}

		if (queue.size() == 0)
			finished = true;
		
		system->update();
	}
}

PianoSong::~PianoSong()
{
	for (int i = 0; i <= 87; i++)
		sounds[i]->release();

	result = system->close();
	result = system->release();
}

bool PianoSong::shouldAddToQueue()
{
	return queue.size() <= 7;
}

void PianoSong::addToQueue(std::istream& infile)
{
	std::string treble, bass;

	while (std::getline(infile, treble).peek() == '\n')
		;

	if (treble.size() == 0)
		std::getline(infile, treble);

	if (isACommentLine(treble))
		return;

	std::getline(infile, bass);

	Measure measure;
	measure.create(treble, bass, noteTypeToSound, system, samplesPerWhole, trebleSample, bassSample);
	queue.push(measure);
}

bool PianoSong::isACommentLine(const std::string& line)
{
	int numFowardSlashes = 0, iter = 0;
	for (auto i = line.cbegin(); i != line.cend() && iter < 2; i++, iter++)
	{
		if (*i == '/')
			numFowardSlashes++;
	}
	return numFowardSlashes == 2;
}

void PianoSong::readBPM(std::istream& infile)
{
	std::string line, strBPM, strBPW;
	infile >> line;
	
	bool finishedBPM(false);
	for (auto i = line.cbegin(); i != line.cend(); i++)
	{
		if (*i == '/')
		{
			finishedBPM = true;
			i++;
		}

		if (finishedBPM)
			strBPW.push_back(*i);
		else
			strBPM.push_back(*i);
	}

	double beatsPerMin(std::stoi(strBPM));
	double beatsPerWhole(std::stoi(strBPW));

	calculateSamplesPerWhole(beatsPerMin, beatsPerWhole);
}

void PianoSong::readSignature(std::istream& infile)
{
	std::string line;
	infile >> line;
}

void PianoSong::calculateSamplesPerWhole(double beatsPerMin, double beatsPerWhole)
{
	samplesPerWhole = (1 / (beatsPerMin / beatsPerWhole / 60 / sampleRate));
}

void PianoSong::createNoteTypeToSound(const std::vector<std::string>& noteTypeVector)
{
	for (int i = 0; i <= 87; i++)
		noteTypeToSound.insert({ noteTypeVector[i], sounds[i] });
}

void PianoSong::createSounds(const std::vector<std::string>& filenameVector)
{
	for (int i = 0; i <= 87; i++)
		system->createSound(filenameVector.at(i).c_str(), FMOD_DEFAULT, NULL, &sounds[i]);
}

std::vector<std::string> PianoSong::filenameVectorCreator()
{
	std::vector<std::string> result;

	int keyNumber = 0;
	for (int i = 0; i <= 87; i++)
	{
		result.push_back(makeWavFilename(getKeyLetter(i), keyNumber));

		keyNumber++;
		if (shouldResetKeyNumber(i))
			resetKeyNumber(keyNumber, i);
	}

	return result;
}

std::vector<std::string> PianoSong::noteTypeArrayCreator()
{
	std::vector<std::string> result;

	int keyNumber = 0;
	for (int i = 0; i <= 87; i++)
	{
		result.push_back(makeNoteType(getKeyLetter(i), keyNumber));

		keyNumber++;
		if (shouldResetKeyNumber(i))
			resetKeyNumber(keyNumber, i);
	}

	return result;
}





bool PianoSong::shouldResetKeyNumber(int index)
{
	switch (index)
	{
	case 7:
	case 15:
	case 23:
	case 30:
	case 37:
	case 44:
	case 51:
	case 58:
	case 66:
	case 73:
	case 80:
		return true;

	default:
		return false;

	}
}

const char* PianoSong::keyNumberToCharStr(int keyNumber)
{
	switch (keyNumber) {
	case 0: return "0";
	case 1: return "1";
	case 2: return "2";
	case 3: return "3";
	case 4: return "4";
	case 5: return "5";
	case 6: return "6";
	case 7: return "7";
	case 8: return "8";
	default: return "Oops"; //should raise an exception
	}
}

const char* PianoSong::getKeyLetter(int index)
{
	if (index >= 0 && index <= 7)
		return "A";

	else if (index >= 8 && index <= 15)
		return "B";

	else if (index >= 16 && index <= 23)
		return "C";

	else if (index >= 24 && index <= 30)
		return "D";

	else if (index >= 31 && index <= 37)
		return "E";

	else if (index >= 37 && index <= 44)
		return "F";

	else if (index >= 45 && index <= 51)
		return "G";

	else if (index >= 52 && index <= 58)
		return "Ab";

	else if (index >= 59 && index <= 66)
		return "Bb";

	else if (index >= 67 && index <= 73)
		return "Db";

	else if (index >= 74 && index <= 80)
		return "Eb";

	else if (index >= 81 && index <= 87)
		return "Gb";

	else
		return "Oops"; //Should raise and exception
}

std::string PianoSong::makeNoteType(const char* keyLetter, int keyNumber)
{
	char theTemplate[4] = "";
	strcat_s(theTemplate, keyLetter);
	strcat_s(theTemplate, keyNumberToCharStr(keyNumber));
	return std::string(theTemplate);
}

std::string PianoSong::makeWavFilename(const char* keyLetter, int keyNumber)
{
	char theTemplate[20] = "ff/Piano.ff.";
	strcat_s(theTemplate, keyLetter);
	strcat_s(theTemplate, keyNumberToCharStr(keyNumber));
	strcat_s(theTemplate, ".wav");

	return std::string(theTemplate);
}

void PianoSong::resetKeyNumber(int& keyNumber, int index)
{
	if ((index <= 7) || (index == 58))
		keyNumber = 0;

	else
		keyNumber = 1;
}