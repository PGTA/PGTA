
#pragma once

#include <memory>

struct PGTATrackSample;

class PGTATrack
{
public:
    PGTATrack();
    ~PGTATrack();

    void SetSamples(std::unique_ptr<PGTATrackSample[]>& samples);

private:
	std::unique_ptr<PGTATrackSample[]> m_samples;
};
