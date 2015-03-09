
#pragma once

#include <private/akPGTATrack.h>
#include <stdint.h>
#include <vector>

struct PGTABuffer;
struct PGTAConfig;

class PGTAContextImpl
{
public:
    PGTAContextImpl();
    ~PGTAContextImpl();

    bool Initialize(const PGTAConfig& config);
    void Shutdown();

    void BindTrack(const PGTATrack* track);

    PGTABuffer* Update(const float delta);

    PGTABuffer* GetOutputBuffers() const;

private:
    std::vector<const PGTATrack*> m_tracks;
};
