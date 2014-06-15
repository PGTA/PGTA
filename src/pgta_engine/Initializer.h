
#pragma once

#include "AudioSample.h"
#include "EngineTrack.h"
#include "WavParser.h"
#include <memory>

namespace Initializer
{
	// make different versions for different audio file formats
	bool InitializeWaveSample(const char *filePath, AudioSample* &sample);
	bool InitializeTrack(const char *trackName, EngineTrack* &track);
}
