#include "stdafx.h"
#include "HalfMeasure.hpp"

HalfMeasure::HalfMeasure()
{
}

HalfMeasure::~HalfMeasure()
{
}

void HalfMeasure::create(const std::string& line, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
	const unsigned long long& samplesPerWhole, unsigned long long& currentSample)
{
	int fowardSlashes = 0;
	std::string noteDescription;
	std::string::const_iterator nextChar = line.cbegin();
	nextChar++;
	for (auto i = line.cbegin(); (fowardSlashes < 2) && (i != line.cend()); i++)
	{
		if (*i == '/')
			fowardSlashes++;
		else if (fowardSlashes == 1)
			fowardSlashes = 0;

		if (fowardSlashes <= 1 && *i != ';' && *i != ' ' && *i != '\t' && *i != '\n')
			noteDescription.push_back(*i);

		if (*i == ';' || (nextChar == line.cend() && fowardSlashes <= 1) || (noteDescription.length() >= 1 && noteDescription != std::string("/") && fowardSlashes >= 2))
		{
			if (fowardSlashes >= 2)
				noteDescription.pop_back();
			createNotes(noteDescription, noteTypeToSound, system, samplesPerWhole, currentSample);
			noteDescription.clear();
		}

		if (nextChar != line.cend())
			nextChar++;
	}
}

void HalfMeasure::createNotes(const std::string& noteDescription, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
	const unsigned long long& samplesPerWhole, unsigned long long& currentSample)
{
	if (noteDescription.find('{') != std::string::npos)
		createMultipleNotes(noteDescription, noteTypeToSound, system, samplesPerWhole, currentSample);

	else
		createOneNote(noteDescription, noteTypeToSound, system, samplesPerWhole, currentSample);
}

void HalfMeasure::getNoteValueAll(std::string::const_iterator& i, double& noteValueAll)
{
	std::string noteValueNumStr, noteValueDenomStr;
	for (; *i != '(' && *i != '{' && *i != '/'; i++)
		noteValueNumStr += *i;

	i++;

	for (; *i != '(' && *i != '{'; i++)
		noteValueDenomStr += *i;

	double noteValueNum(std::atoi(noteValueNumStr.c_str()));
	double noteValueDenom = 1.0;
	if (noteValueDenomStr.size() > 0)
		noteValueDenom = std::atoi(noteValueDenomStr.c_str());
	noteValueAll = noteValueNum / noteValueDenom;
}

void HalfMeasure::getOffset(std::string::const_iterator& i, double& offset)
{
	i++;
	std::string offsetNumStr, offsetDenomStr;

	for (; *i != '/'; i++)
		offsetNumStr += *i;

	if (*i == '/')
		i++;

	for (; *i != ')'; i++)
		offsetDenomStr += *i;

	double offsetNum(std::atoi(offsetNumStr.c_str()));
	double offsetDenom = 1;
	if (offsetDenomStr.size() > 0)
		offsetDenom = std::atoi(offsetDenomStr.c_str());

	offset = offsetNum / offsetDenom;
	i++;
}

void HalfMeasure::createMultipleNotes(const std::string& notesDescription, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
	const unsigned long long& samplesPerWhole, unsigned long long& currentSample)
{
	auto i = notesDescription.cbegin();

	//Getting the note value outside the ()'s & {}'s=================================================
	double noteValueAll = 1;
	if (*i != '{' && *i != '(')
		getNoteValueAll(i, noteValueAll);
	
	//Getting the note value inside ()'s================================================================
	double offset = 0;
	if (*i == '(')
		getOffset(i, offset);

	//Inside the {}'s ================================================================================
	unsigned long long newCurrentSample = currentSample;
	parseBrackets(i, notesDescription, noteValueAll, offset, newCurrentSample, noteTypeToSound, system, samplesPerWhole, currentSample);
	currentSample = newCurrentSample;
}

void HalfMeasure::parseBrackets(std::string::const_iterator& i, const std::string& notesDescription, double& noteValueAll, double& offset, unsigned long long& newCurrentSample,
	const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system, const unsigned long long& samplesPerWhole, unsigned long long& currentSample)
{
	i++;
	std::string noteTypeStr;
	bool hasBracket(false), isBracket(false);
	double noteValue = 1, bracketValue = 0;
	int iteration = 0;
	for (; i != notesDescription.cend(); i++)
	{
		for (; *i != ',' && *i != '}' && *i != '/' && *i != '['; i++)
			noteTypeStr += *i;

		noteValue = 1;
		if (*i != ',' && *i != '}')
		{
			getNoteValue(i, noteValue);

			if (*i == '[')
				getBracketValue(i, bracketValue, hasBracket, isBracket);
		}

		unsigned long long currentSampleWithOffset(currentSample + (samplesPerWhole * offset * iteration));
		createNote(noteTypeToSound.at(noteTypeStr), (noteValueAll * noteValue) - (offset * iteration), system, samplesPerWhole, currentSampleWithOffset);

		if (hasBracket && isBracket)
		{
			newCurrentSample = currentSampleWithOffset - (bracketValue * samplesPerWhole);
			isBracket = false;
		}

		else if (!hasBracket && (currentSampleWithOffset > newCurrentSample))
			newCurrentSample = currentSampleWithOffset;

		noteTypeStr.clear();
		iteration++;
	}
}

void HalfMeasure::getBracketValue(std::string::const_iterator& i, double& bracketValue, bool& hasBracket, bool& isBracket)
{
	hasBracket = true;
	isBracket = true;
	i++;
	std::string bracketNumStr, bracketDenomStr;
	for (; *i != '/' && *i != ']'; i++)
		bracketNumStr += *i;
	i++;
	for (; *i != ']' && *i != ','; i++)
		bracketDenomStr += *i;
	double bracketNumValue(std::atoi(bracketNumStr.c_str()));

	double bracketDenomValue = 1;
	if (bracketDenomStr.size() > 0)
		bracketDenomValue = std::atoi(bracketDenomStr.c_str());

	bracketValue = bracketNumValue / bracketDenomValue;
	i++;
}


void HalfMeasure::getNoteValue(std::string::const_iterator& i, double& noteValue)
{
	std::string noteValueNumStr, noteValueDenomStr;
	for (; *i != ',' && *i != '}' && *i != '/' && *i != '['; i++)
		noteValueNumStr += *i;

	if (*i == '/')  //Test further=================================================================
		i++;

	for (; *i != ',' && *i != '}' && *i != '['; i++)
		noteValueDenomStr += *i;

	double numValue = 1, denomValue = 1;
	if (noteValueNumStr.size() > 0)
		numValue = std::atoi(noteValueNumStr.c_str());

	if (noteValueDenomStr.size() > 0)
		denomValue = std::atoi(noteValueDenomStr.c_str());

	noteValue = numValue / denomValue;
}

void HalfMeasure::getNoteType(std::string::const_iterator& i, std::string& noteType, const std::string& noteDescription, bool& isRest)
{
	bool haveNoteType(false);
	for (; !isRest && !haveNoteType && i != noteDescription.cend() && *i != '/' && *i != '['; i++)
	{
		noteType += *i;

		if (*i == 'R')
			isRest = true;

		if (isInt(*i))
			haveNoteType = true;
	}
}

void HalfMeasure::getNoteValue(std::string::const_iterator& i, const std::string& noteDescription, double& noteValue)
{
	if (i == noteDescription.cend())
		noteValue = 1.0;

	else
	{
		std::string numValueStr, denomValueStr;
		double numValue = 1.0, denomValue = 1.0;

		for (; *i != '/' && *i != '['; i++)
			numValueStr += *i;

		if (i != noteDescription.cend() && *i == '/')
			i++;

		for (; i != noteDescription.cend() && *i != '['; i++)
			denomValueStr += *i;

		if (numValueStr.size() > 0)
			numValue = std::atoi(numValueStr.c_str());

		if (denomValueStr.size() > 0)
			denomValue = std::atoi(denomValueStr.c_str());

		noteValue = numValue / denomValue;
	}
}

void HalfMeasure::getBracketValue(std::string::const_iterator& i, const std::string& noteDescription, double& bracketValue)
{
	if (i == noteDescription.cend())
		bracketValue = 0.0;

	else
	{
		i++;

		std::string bracketNumStr, bracketDenomStr;
		double bracketNum = 0.0, bracketDenom = 1.0;

		for (; *i != '/' && *i != ']'; i++)
			bracketNumStr += *i;

		if (i != noteDescription.cend())
			i++;

		for (; i != noteDescription.cend() && *i != ']'; i++)
			bracketDenomStr += *i;

		if (bracketNumStr.size() > 0)
			bracketNum = std::atoi(bracketNumStr.c_str());

		if (bracketNumStr.size() > 0)
			bracketDenom = std::atoi(bracketDenomStr.c_str());

		bracketValue = bracketNum / bracketDenom;
	}
}

void HalfMeasure::createOneNote(const std::string& noteDescription, const std::unordered_map<std::string, FMOD::Sound*>& noteTypeToSound, FMOD::System* system,
	const unsigned long long& samplesPerWhole, unsigned long long& currentSample)
{
	std::string noteType;

	auto i = noteDescription.cbegin();

	//Get NoteType===================================================================
	
	bool isRest(false);
	getNoteType(i, noteType, noteDescription, isRest);


	//Get NoteValue=======================================================================
	double noteValue;
	getNoteValue(i, noteDescription, noteValue);

	//If it's a rest=====================================================================

	if (isRest)
	{
		currentSample += (noteValue * samplesPerWhole);
		return;
	}

	//Get Bracket Value=================================================================

	double bracketValue;
	getBracketValue(i, noteDescription, bracketValue);

	//Create the note=================================================================

	createNote(noteTypeToSound.at(noteType), noteValue, system, samplesPerWhole, currentSample);
	currentSample -= (bracketValue * samplesPerWhole);
}

void HalfMeasure::createNote(FMOD::Sound* sound, double noteValue, FMOD::System* system, const unsigned long long& samplesPerWhole,
	unsigned long long& currentSample)
{
	Note note(noteValue);
	note.create(system, sound, false, currentSample, samplesPerWhole);
	notes.push_back(note);
}

void HalfMeasure::setPause(bool paused)
{
	for (auto i = notes.begin(); i != notes.end(); i++)
		i->setPaused(paused);
}


bool HalfMeasure::finishedPlaying() const
{
	for (auto i = notes.cbegin(); i != notes.cend(); i++)
	{
		if (i->isPlaying())
			return false;
	}
	return true;
}

bool HalfMeasure::isInt(char character)
{
	switch (character)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return true;

	default:
		return false;
	}
}