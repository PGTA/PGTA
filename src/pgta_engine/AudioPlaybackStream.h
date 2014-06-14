
#pragma once

#include "AudioStreamBuffer.h"
#include "OALWrapper/OAL_Funcs.h"
#include "OALWrapper/OAL_Stream.h"
#include "OALWrapper/OAL_Buffer.h"

class AudioPlaybackStream
{
public:
    AudioPlaybackStream(int bitsPerSample):
        m_inputStream(nullptr),
        m_oalStream(nullptr),
        m_sourceId(0),
        m_numChannels(0),
        m_frequency(0),
        m_format(0),
        m_bytesPerSample(bitsPerSample / 8)
    {
    }
    
    // InitStream(2, 44100, AL_FORMAT_STEREO16);
    bool InitStream(ALint channels, ALint frequency, ALenum format)
    {
        if (m_oalStream)
        {
            return false;
        }
        
        tStreamCallbacks callbacks = {};
        callbacks.Init = &AudioPlaybackStream::callbackInit;
        callbacks.GetTime = &AudioPlaybackStream::callbackGetTime;
        callbacks.Seek = &AudioPlaybackStream::callbackSeek;
        callbacks.Stream = &AudioPlaybackStream::callbackStream;
        callbacks.Destroy = &AudioPlaybackStream::callbackDestroy;
        
        tStreamInfo info = {};
        info.channels = channels;
        info.frequency = frequency;
        info.format = format;
        
        m_oalStream = OAL_Stream_LoadCustom(callbacks, info, this);
        m_sourceId = OAL_Stream_Play(OAL_FREE, m_oalStream, 1.0, false);
        
        return (m_oalStream != nullptr);
    }
    
    void DestroyStream()
    {
        OAL_Source_Stop(m_sourceId);
        OAL_Stream_Unload(m_oalStream);
        m_sourceId = 0;
    }
    
    bool IsPlaying() const
    {
        return OAL_Source_IsPlaying(m_sourceId);
    }
    
    int GetNumChannels() const
    {
        return m_numChannels;
    }
    
    int GetFrequency() const
    {
        return m_frequency;
    }
    
    ALenum GetFormat() const
    {
        return m_format;
    }
    
    void SetInputStream( AudioStreamBuffer* inputStream )
    {
        // todo assert stream format
        m_inputStream = inputStream;
    }
    
private:
    void Init() {}
    double GetTime()
    {
        return 0.0;
    }
    void Seek(float afWhere, bool abForceRebuffer) {}
    bool Stream(cOAL_Buffer* apDestBuffer, char *buffer,
                unsigned int bufferSize, bool& eof)
    {
        int givenSamples = m_inputStream->PullSamples(buffer, bufferSize / m_bytesPerSample);
        if (givenSamples <= 0)
        {
            //eof = true; // find a way to terminate stream (thread safe)
            return true;
        }
        apDestBuffer->Feed(buffer, givenSamples * m_bytesPerSample);
        return true;
    }
    void Destroy() {}
    
private:
    // callbacks for OALWrapper
    static void callbackInit(void *data)
    {
        ((AudioPlaybackStream*)data)->Init();
    }
    
    static double callbackGetTime(void *data)
    {
        return ((AudioPlaybackStream*)data)->GetTime();
    }
    
    static void callbackSeek(void *data, float afWhere, bool abForceRebuffer)
    {
        ((AudioPlaybackStream*)data)->Seek(afWhere, abForceRebuffer);
    }
    
    static bool callbackStream(void *data, cOAL_Buffer* apDestBuffer,
                               char *buffer, unsigned int bufferSize, bool& eof)
    {
        return ((AudioPlaybackStream*)data)->Stream(apDestBuffer, buffer, bufferSize, eof);
    }
    
    static void callbackDestroy(void *data)
    {
        ((AudioPlaybackStream*)data)->Destroy();
    }
    
private:
    AudioStreamBuffer *m_inputStream;
    cOAL_Stream *m_oalStream;
    int m_sourceId;
    int m_numChannels;
    int m_frequency;
    ALenum m_format;
    int m_bytesPerSample;
};
