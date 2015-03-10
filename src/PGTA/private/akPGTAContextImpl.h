
#pragma once

#include <private/akPGTATrack.h>
//#include <private/PGTAScheduler.h>
#include <stdint.h>
#include <memory>
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

    PGTABuffer* GetOutputBuffer() const;

private:
    //std::unique_ptr<PGTAScheduler> m_scheduler;
    PGTAConfig m_config;

};
