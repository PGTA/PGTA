
#include "AudioPlaybackStream.h"
#include "AudioStreamBuffer.h"
#include "OALWrapper/OAL_Funcs.h"

#include "utils.h"
#include "IPGTA.h"
#include <iostream>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

void FixWorkingDirectory()
{
    char temp[128] = {};
    const char *dir = getcwd(temp, sizeof(temp));
    const char *bin_pos = strstr(dir, "bin");
    const char *build_pos = strstr(dir, "build");
    if (bin_pos)
    {
        chdir("..");
    }
    else if (build_pos)
    {
        chdir("../..");
    }
}

PGTA::IPGTA* SetupPGTA()
{
    using namespace PGTA;

    IPGTA* pgta = CreatePGTA();

    PGTAConfig config;
    config.audioDesc.samplesPerSecond = 44100;
    config.audioDesc.bytesPerSample = 2;
    config.audioDesc.channels = 1;
    config.numBuffers = 4;
    config.bufferSizeInSamples = 8192;

    pgta->Initialize(config);
    return pgta;
}

void SetupOAL()
{
    cOAL_Init_Params oal_parms;
    oal_parms.mlStreamingBufferSize = 8192;
    if (OAL_Init(oal_parms) == false)
    {
        printf("OAL Failed - Check your OpenAL installation\n");
    }
    else
    {
        printf("OAL Success\n");
    }
}

#include <SDL.h>

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    FixWorkingDirectory();
    SetupOAL();

    AudioStreamBuffer pgtaOutput(16, true);
    {
        std::unique_ptr<char[]> temp(new char[8192]());
        pgtaOutput.PushSamples(temp.get(), 4096);
    }
    AudioPlaybackStream playbackStream(16);
    playbackStream.SetInputStream(&pgtaOutput);
    playbackStream.InitStream(1, 44100, AL_FORMAT_MONO16);

    auto* pgta = SetupPGTA();
    if (!pgta->StartPlayback("tracks/ambient_rain.track"))
    {
        return -1;
    }

    utils::RunLoop(10.0f, [&]
    {
        if (!playbackStream.IsPlaying())
        {
            return false;
        }

        pgta->Update();

        int numBuffers = 0;
        const auto* buffers = pgta->GetOutputBuffers(numBuffers);
        for (int i = 0; i < numBuffers; ++i)
        {
            auto& buf = buffers[i];
            pgtaOutput.PushSamples(buf.audioData, buf.numSamples);
        }

        //pgta->StopPlayback();
        return true;
    });

    PGTA::FreePGTA(pgta);

    playbackStream.DestroyStream();
    OAL_Close();
    SDL_Quit();
    return 0;
}
