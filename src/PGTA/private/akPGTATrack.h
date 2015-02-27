
#pragma once

#include <memory>

struct PGTATrackSample;

class PGTATrack
{
public:
    PGTATrack();
    ~PGTATrack();

    void SetSamples(std::unique_ptr<PGTATrackSample[]>& samples, const uint16_t numSamples);
    uint16_t GetNumberSamples() const { return m_numSamples; };
    PGTATrackSample* CopySampleData() const;

private:
	std::unique_ptr<PGTATrackSample[]> m_samples;
    uint16_t m_numSamples;
};
