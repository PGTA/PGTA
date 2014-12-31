
#include <iostream>
#include <string.h>
#include <SDL.h>
#include <akPGTA.h>

#include "FileUtils.h"
#include "utils.h"

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
    std::string trackSource;
    if (!ReadBinaryFileToString("tracks/demo.track", trackSource))
    {
        return -1;
    }

    HPGTATrack demoTrack = nullptr;
    const char* source = trackSource.data();
    if (pgtaDevice.CreateTracks(1, &source, &demoTrack) <= 0 || !demoTrack)
    {
        return -1;
    }

    PGTAConfig config;
    config.audioDesc.samplesPerSecond = 44100;
    config.audioDesc.bytesPerSample = 2;
    config.audioDesc.channels = 1;
    config.numBuffers = 4;
    config.bufferSizeInSamples = 8192;
    PGTA::PGTAContext pgtaContext(pgtaDevice.CreateContext(config));

    //pgtaContext.BindTrack(demoTrack);

    utils::RunLoop(10.0f, [&]
    {
        int32_t numBuffers = 0;
        const PGTABuffer* buffers = pgtaContext.Update(10.0f, &numBuffers);
        for (int32_t i = 0; i < numBuffers; ++i)
        {
            auto& buf = buffers[i];
            // use samples
        }

        return (buffers != nullptr);
    });

    SDL_Quit();
    return 0;
}
