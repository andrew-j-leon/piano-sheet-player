// Piano.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <Windows.h>
#include <mmsystem.h>

#include <fmod.hpp>
#include <fmod.h>

#include "FMODError.hpp"
#include "PianoSong.hpp"


int main()
{
	try
	{
		PianoSong piano;
		piano.play("example1.txt");
	}

	catch (const FMODError& fmodError)
	{
		fmodError.message();
	}

	return 0;
}