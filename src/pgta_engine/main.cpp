
#include "AudioData.h"
#include "WavParser.h"
#include "AudioPlaybackStream.h"
#include "AudioStreamBuffer.h"
#include "AudioMixer.h"
#include "OALWrapper/OAL_Funcs.h"
#include "SDL_timer.h"
#include <iostream>
#include <vector>

int main( int argc, char *argv[] )
{
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
    
    AudioPlaybackStream stream(16);
    AudioMixer<16> mixer;

    std::vector<audio_data> audioData;
    std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers;
    for (int i = 1; i < argc; ++i)
    {
        audioData.emplace_back();
        auto &data = audioData.back();
        ParseWaveFile(argv[i], data);

        std::cout << "Playing : " << argv[i] << std::endl;
        std::cout << "Channels : " << data.channels << std::endl;
        std::cout << "Sample-rate : " << data.samplesPerSecond << std::endl;
        std::cout << "Bits-per-sample : " << data.bitsPerSample << std::endl;

        streamBuffers.emplace_back(new AudioStreamBuffer(data.bitsPerSample));
        streamBuffers.back()->PushSamples(data.samples.get(), data.numSamples);

        mixer.ConnectInputStream(streamBuffers.back().get());
    }

    AudioStreamBuffer mixerOutput(16, true);
    mixer.ConnectOutputStream(&mixerOutput);

    mixer.Mix();

    stream.SetInputStream(&mixerOutput);
    stream.InitStream(1, 44100, AL_FORMAT_MONO16);
    
    while (stream.IsPlaying())
    {
        if (mixerOutput.GetNumSamples() == 0)
        {
            for (int i = 0; i < streamBuffers.size(); ++i)
            {
                streamBuffers[i]->PushSamples(audioData[i].samples.get(), audioData[i].numSamples);
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
    OAL_Close ();
    return 0;
}
