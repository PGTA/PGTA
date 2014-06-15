
#include "AudioData.h"
#include "WavParser.h"
#include "AudioPlaybackStream.h"
#include "AudioStreamBuffer.h"
#include "AudioMixer.h"
#include "EngineTrack.h"
#include "EngineSample.h"
#include "Initializer.h"
#include "OALWrapper/OAL_Funcs.h"
#include "SDL_timer.h"
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

int main( int argc, char *argv[] )
{
    // fix up working directory
    {
        char temp[128] = {};
        const char *dir = _getcwd(temp, sizeof(temp));
        const char *bin_pos = strstr(dir, "bin");
        const char *build_pos = strstr(dir, "build");
        if (bin_pos)
        {
            _chdir("..");
        }
        else if (build_pos)
        {
            _chdir("../..");
        }
    }
    // OAL_SetupLogging(true,eOAL_LogOutput_File,eOAL_LogVerbose_High);

    cOAL_Init_Params oal_parms;
    oal_parms.mlStreamingBufferSize = 8192;
    if (OAL_Init(oal_parms)==false)
    {
        printf ("Failed - Check your OpenAL installation\n");
        return 0;
    }
    else
    {
        printf ("Success\n");
    }

    AudioMixer<16> mixer;
    std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers;

    std::unique_ptr<EngineTrack> ambientRain(Initializer::InitializeTrack("tracks/ambient_rain.track"));
    if (!ambientRain)
    {
        return -1;
    }

    for (auto &sample : ambientRain->getSamples())
    {
        const AudioSample *audioSample = sample.getSample();
        streamBuffers.emplace_back(new AudioStreamBuffer(audioSample->getBitsPerSample()));
        streamBuffers.back()->PushSamples(audioSample->getSamples(), audioSample->getNumSamples());
        mixer.ConnectInputStream(streamBuffers.back().get());
    }

    AudioStreamBuffer mixerOutput(16, true);
    mixer.ConnectOutputStream(&mixerOutput);

    mixer.Mix();

    AudioPlaybackStream stream(16);
    stream.SetInputStream(&mixerOutput);
    stream.InitStream(1, 44100, AL_FORMAT_MONO16);

    while (stream.IsPlaying())
    {
        if (mixerOutput.GetNumSamples() <= oal_parms.mlStreamingBufferSize)
        {
            for (int i = 0; i < (int)streamBuffers.size(); ++i)
            {
                const AudioSample *audioSample = ambientRain->GetEngineSample(i).getSample();
                streamBuffers[i]->PushSamples(audioSample->getSamples(), audioSample->getNumSamples());
                std::cout << streamBuffers[i]->GetNumSamples() << std::endl;
            }
            mixer.Mix();
        }
        else
        {
            SDL_Delay(10);
        }
        // sleep if you want
    }

    stream.DestroyStream();
    OAL_Close();
    return 0;
}
