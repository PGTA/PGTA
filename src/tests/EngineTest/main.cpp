
#include <SDL.h>
#include <SDL_audio.h>
#include <akAudioMixer.h>
#include <assert.h>
#include <signal.h>
#include <iostream>
#include <akPGTA.h>
#include <chrono>
#include "utils.h"
#include "FileUtils.h"

class SDLWav
{
public:
    SDLWav(const char* filename) :
        m_audioBuf(nullptr),
        m_audioLen(0),
        m_spec()
    {
        SDL_LoadWAV(filename, &m_spec, &m_audioBuf, &m_audioLen);
        std::cout << filename << std::endl;
        assert(m_spec.freq == 44100);
        assert(m_spec.format == AUDIO_S16);
        assert(m_spec.channels == 1);
        std::cout << filename << " has " << (m_audioLen >> 1) << " samples\n";
    }

    ~SDLWav()
    {
        if (m_audioBuf)
        {
            SDL_FreeWAV(m_audioBuf);
        }
    }

    explicit operator bool() const
    {
        return (m_audioBuf != nullptr && m_audioLen > 0);
    }

    const SDL_AudioSpec& GetSpec() const
    {
        return m_spec;
    }

    const uint8_t* GetSamplePtr() const
    {
        return reinterpret_cast<const uint8_t*>(m_audioBuf);
    }

    uint32_t GetNumSamples() const
    {
        assert((m_audioLen % 2) == 0);
        return (m_audioLen >> 1);
    }

private:
    Uint8* m_audioBuf;
    Uint32 m_audioLen;
    SDL_AudioSpec m_spec;
};

class CAverageValueSampler
{
public:
    CAverageValueSampler():
        m_samples(32, 0.0),
        m_runningTotal(0.0),
        m_iCurSample(0)
    {
    }

    void TakeSample( double sample )
    {
        m_iCurSample %= 32;
        
        m_runningTotal -= m_samples[m_iCurSample];
        m_runningTotal += sample;
        
        m_samples[m_iCurSample++] = sample;
    }
    
    double GetAverage() const
    {
        return m_runningTotal / 32.0;
    }
    
private:
    std::vector<double> m_samples;
    double m_runningTotal;
    uint8_t m_iCurSample;
};

int pgtaMain(SDL_AudioDeviceID audioDevice)
{
    PGTA::PGTADevice pgtaDevice;
    if (!pgtaDevice.Initialize())
    {
        return -1;
    }

    // load track data to memory
    // "tracks/test.track"
    std::string trackSource;
    if (!ReadBinaryFileToString("tracks/test.track", trackSource))
    {
        return -1;
    }

    HPGTATrack demoTrack1 = nullptr;
    const char* source = trackSource.data();
    const size_t length = trackSource.length();
    if (pgtaDevice.CreateTracks(1, &source, &length, &demoTrack1) <= 0 || !demoTrack1)
    {
        return -1;
    }

    const PGTATrackData trackData = pgtaGetTrackData(demoTrack1);
    const int numSamples = trackData.numSamples;

    std::vector<SDLWav> audioFiles;
    audioFiles.reserve(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        const PGTASampleData* sampleData = (trackData.samples + i);
        audioFiles.emplace_back(sampleData->defaultFile);
        pgtaBindTrackSample(demoTrack1, sampleData->id,
                            audioFiles[i].GetSamplePtr(),
                            audioFiles[i].GetNumSamples() * 2);
    }

    // load track data to memory
    // "tracks/upbeat.track"
    std::string trackSource2;
    if (!ReadBinaryFileToString("tracks/upbeat.track", trackSource2))
    {
        return -1;
    }

    HPGTATrack demoTrack2 = nullptr;
    const char* source2 = trackSource2.data();
    const size_t length2 = trackSource2.length();
    if (pgtaDevice.CreateTracks(1, &source2, &length2, &demoTrack2) <= 0 || !demoTrack2)
    {
        return -1;
    }

    const PGTATrackData trackData2 = pgtaGetTrackData(demoTrack2);
    const int numSamples2 = trackData2.numSamples;

    std::vector<SDLWav> audioFiles2;
    audioFiles2.reserve(numSamples2);
    for (int i = 0; i < numSamples2; ++i)
    {
        const PGTASampleData* sampleData = (trackData2.samples + i);
        audioFiles2.emplace_back(sampleData->defaultFile);
        pgtaBindTrackSample(demoTrack2, sampleData->id,
            audioFiles2[i].GetSamplePtr(),
            audioFiles2[i].GetNumSamples() * 2);
    }

    PGTAConfig config;
    config.audioDesc.samplesPerSecond = 44100;
    config.audioDesc.bytesPerSample = 2;
    config.audioDesc.channels = 1;
    config.beatsPerMinute = 120;
    config.mixAhead = 0.1f;
    PGTA::PGTAContext pgtaContext(pgtaDevice.CreateContext(config));

    pgtaContext.BindTrack(demoTrack2);

    uint64_t count = 0;
    CAverageValueSampler avgValue;
    utils::RunLoop(0.01f, [&](double /*absoluteTime*/, float delta)
    {
        using namespace std::chrono;
        count = ((count + 1) % 2000);
        if (count == 0)
        {
            pgtaContext.Transition(demoTrack1, 1.0f, 10.0f);
            std::swap(demoTrack1, demoTrack2);
        }
        auto asdf = high_resolution_clock::now();
        const PGTABuffer output = pgtaContext.Update(delta);
        auto asdf2 = high_resolution_clock::now();
        duration<double> curTimeDouble = duration_cast<duration<double>>(asdf2 - asdf);
        avgValue.TakeSample(curTimeDouble.count());
        std::cout << "Time: " << avgValue.GetAverage() << "\n";
        SDL_QueueAudio(audioDevice, output.samples, static_cast<Uint32>(output.numSamples * 2));
        return (output.samples != nullptr) && (output.numSamples > 0);
    });
    return 0;
}

static void quit(int /*param*/)
{
    exit(1);
}

int main()
{
    std::cout.sync_with_stdio(false);
    signal(SIGINT, &quit);
    utils::FixWorkingDirectory();
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_AudioSpec audioSpec;
    SDL_zero(audioSpec);
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = 1;
    audioSpec.samples = 4096;
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(nullptr, false, &audioSpec, nullptr, 0);

    int ret = 0;
    if (audioDevice > 1)
    {
        SDL_PauseAudioDevice(audioDevice, 0);
        ret = pgtaMain(audioDevice);
        SDL_CloseAudioDevice(audioDevice);
    }

    SDL_Quit();
    return ret;
}
