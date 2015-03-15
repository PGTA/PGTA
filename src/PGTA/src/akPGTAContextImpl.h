
#pragma once

#include "akPGTATrack.h"
#include "akPGTAScheduler.h"
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

    PGTABuffer Update(const float delta);

    PGTABuffer GetOutputBuffer() const;

private:
    PGTABuffer m_outputBuffer;
    std::unique_ptr<PGTAScheduler> m_scheduler;
    PGTAConfig m_config;

};
