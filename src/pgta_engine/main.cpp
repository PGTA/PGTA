
#include "AudioPlaybackStream.h"
#include "AudioStreamBuffer.h"
#include "OALWrapper/OAL_Funcs.h"

#include "utils.h"
#include "IPGTA.h"

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

IPGTA* SetupPGTA()
{
    IPGTA* pgta = IPGTA::CreatePGTA();

    PGTAConfig config;
    config.samplesPerSecond = 44100;
    config.bitsPerSample = 16;
    config.channels = 1;
    config.bufferSize = 8192;

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

int main(int argc, char *argv[])
{
    FixWorkingDirectory();
    SetupOAL();

    AudioStreamBuffer pgtaOutput(16);
    AudioPlaybackStream playbackStream(16);
    playbackStream.SetInputStream(&pgtaOutput);
    playbackStream.InitStream(1, 44100, AL_FORMAT_MONO16);

    IPGTA* pgta = SetupPGTA();
    pgta->StartPlayback("tracks/demo.track");

    utils::RunLoop(10.0f, [&]
    {
        if (!playbackStream.IsPlaying())
        {
            return false;
        }

        pgta->Update();

        int numSamples = 0;
        const char *buf = pgta->GetAudioBuffer(numSamples);
        pgtaOutput.PushSamples(buf, numSamples);

        pgta->StopPlayback();
        return true;
    });

    IPGTA::FreePGTA(pgta);

    playbackStream.DestroyStream();
    OAL_Close();
    return 0;
}
