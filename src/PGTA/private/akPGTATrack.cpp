
#include <private/akPGTATrack.h>
#include <public/akPGTATypes.h>

PGTATrack::PGTATrack()
{
}

PGTATrack::~PGTATrack()
{
}

void PGTATrack::SetSamples(std::unique_ptr<PGTATrackSample[]>& samples, uint16_t numSamples)
{
    m_samples = std::move(samples);
    m_numSamples = numSamples;
}

PGTATrackSample* PGTATrack::CopySampleData() const
{
    PGTATrackSample* copySamples = new PGTATrackSample[m_numSamples];
    memcpy(copySamples, m_samples.get(), sizeof(PGTATrackSample)* m_numSamples);
    for (int i = 0; i < m_numSamples; ++i)
    {

        PGTATrackSample &sample = copySamples[i];
        PGTATrackSample &sourceSample = m_samples.get()[i];

        size_t length = strlen(sourceSample.sampleName);
        sample.sampleName = new char[length + 1];
        strcpy(sample.sampleName, sourceSample.sampleName);

        length = strlen(sourceSample.defaultFile);
        sample.defaultFile = new char[length + 1];
        strcpy(sample.defaultFile, sourceSample.defaultFile);

        int numGroups = sourceSample.numGroups;
        if (numGroups <= 0)
        {
            continue;
        }
        sample.groups = new PGTAUUID[numGroups];
        for (int j = 0; j < numGroups; ++j)
        {
            memcpy(sample.groups[j].bytes, &sourceSample.groups[j].bytes, sizeof(sample.groups[j].bytes));
        }
    }
    return copySamples;
}
