#pragma once

#ifndef FMODERROR_HPP
#define FMODERROR_HPP

#include <fmod.h>
#include <fmod.hpp>

class FMODError
{
public:
	FMODError(const FMOD_RESULT& theResult);
	void message() const;

private:
	FMOD_RESULT result;

	const char* resultToCharStr(const FMOD_RESULT& errcode) const;
};

#endif 