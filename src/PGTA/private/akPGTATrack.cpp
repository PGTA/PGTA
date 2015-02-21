
#include <private/akPGTATrack.h>
#include <private/akPGTASample.h>

PGTATrack::PGTATrack()
{
}

PGTATrack::~PGTATrack()
{
}

void PGTATrack::SetSamples(std::unique_ptr<PGTATrackSample[]>& samples)
{
    m_samples = std::move(samples);
}
