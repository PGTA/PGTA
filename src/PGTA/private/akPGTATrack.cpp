
#include <private/akPGTATrack.h>

PGTATrack::PGTATrack()
{
}

PGTATrack::~PGTATrack()
{
}

void PGTATrack::setSamples(PGTATrackSample samples[])
{
    m_samples = std::unique_ptr<PGTATrackSample[]>(samples);
}
