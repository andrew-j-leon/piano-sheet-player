#pragma once

#ifndef HALFMEASURE_HPP
#define HALFMEASURE_HPP

#include <fstream>
#include <vector>
#include <unordered_map>
#include "Note.hpp"

class HalfMeasure
{
public:
	HalfMeasure();
	~HalfMeasure();

	void create(const std::string& line, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
		const unsigned long long& samplesPerWhole, unsigned long long& currentSample);

	void createNotes(const std::string& noteDescription, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
		const unsigned long long& samplesPerWhole, unsigned long long& currentSample);

	void createMultipleNotes(const std::string& notesDescription, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
		const unsigned long long& samplesPerWhole, unsigned long long& currentSample);

	void createNote(FMOD::Sound* sound, double noteValue, FMOD::System* system, const unsigned long long& samplesPerWhole, unsigned long long& currentSample);
	void createOneNote(const std::string& noteDescription, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
		const unsigned long long& samplesPerWhole, unsigned long long& currentSample);
	
	void setPause(bool paused);

	bool finishedPlaying() const;
	


	static bool isInt(char character);
	static void getNoteValueAll(std::string::const_iterator& i, double& noteValueAll);
	static void getOffset(std::string::const_iterator& i, double& offset);

	static void getNoteValue(std::string::const_iterator& i, double& noteValue);
	static void getNoteValue(std::string::const_iterator& i, const std::string& noteDescription, double& noteValue);

	static void getBracketValue(std::string::const_iterator& i, double& bracketValue, bool& hasBracket, bool& isBracket);
	static void getBracketValue(std::string::const_iterator& i, const std::string& noteDescription, double& bracketValue);

	void parseBrackets(std::string::const_iterator& i, const std::string& notesDescription, double& noteValueAll, double& offset, unsigned long long& newCurrentSample,
		const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system, const unsigned long long& samplesPerWhole, unsigned long long& currentSample);

	void getNoteType(std::string::const_iterator& i, std::string& noteType, const std::string& noteDescription, bool& isRest);
	

private:
	std::vector<Note> notes;
};


#endif
