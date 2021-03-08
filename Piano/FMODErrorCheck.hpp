#pragma once

#ifndef FMODERRORCHECK_HPP
#define FMODERRORCHECK_HPP

#include <fmod.h>
#include <fmod.hpp>
#include "FMODError.hpp"

void ERRCHECK(const FMOD_RESULT& result)
{
	if (result != FMOD_OK)
		throw FMODError{ result };
}

#endif