
#pragma once

//#include "AudioData.h"
struct audio_data;

bool ParseWaveFile( const char *filePath, audio_data &dest );
