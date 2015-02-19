
#pragma once

#include <private/akPGTASample.h>
#include <vector>

class PGTATrack
{
public:
    PGTATrack();
    ~PGTATrack();

	void setSamples(std::vector<PGTATrackSample> &samples);

private:
	std::vector<PGTATrackSample> m_samples;
};
