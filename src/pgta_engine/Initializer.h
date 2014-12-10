
#pragma once

#include "AudioSample.h"
#include "EngineTrack.h"

namespace Initializer
{
	// make different versions for different audio file formats
	AudioSample* InitializeWaveSample(const char *filePath);
	EngineTrack* InitializeTrack(const char *trackName);
}
