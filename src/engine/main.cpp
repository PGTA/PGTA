
#include <iostream>
#include <string.h>
#include <SDL.h>
#include <akPGTA.h>

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
/*
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
}*/

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    FixWorkingDirectory();

    PGTA::PGTADevice pgtaDevice;
    if (!pgtaDevice.Initialize())
    {
        return -1;
    }

    // load track data to memory
    // "tracks/demo.track"

    HPGTATrack demoTrack = nullptr;
    pgtaDevice.CreateTracks(1, nullptr, &demoTrack);
    if (!demoTrack)
    {
        return -1;
    }

    /*

    pgta->PlayTrack(demoTrack);
    pgta->StartPlayback();

    utils::RunLoop(10.0f, [&]
    {
        int numBuffers = 0;
        const auto* buffers = pgta->Update(numBuffers);
        for (int i = 0; i < numBuffers; ++i)
        {
            auto& buf = buffers[i];
            pgtaOutput.PushSamples(buf.audioData, buf.numSamples);
        }

        //pgta->StopPlayback();
        return playbackStream.IsPlaying();
    });

    PGTA::FreePGTA(pgta);

    playbackStream.DestroyStream();*/
    SDL_Quit();
    return 0;
}
