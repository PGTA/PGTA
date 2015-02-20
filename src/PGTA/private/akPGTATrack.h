
#pragma once

#include <private/akPGTASample.h>
#include <memory>

class PGTATrack
{
public:
    PGTATrack();
    ~PGTATrack();

	void setSamples(PGTATrackSample samples[]);

private:
	std::unique_ptr<PGTATrackSample[]> m_samples;
};
