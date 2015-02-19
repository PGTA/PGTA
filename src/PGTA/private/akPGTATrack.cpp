
#include <private/akPGTATrack.h>

PGTATrack::PGTATrack()
{
}

PGTATrack::~PGTATrack()
{
}

void PGTATrack::setSamples(std::vector<PGTATrackSample> &samples)
{
    m_samples = std::move(samples);
}
