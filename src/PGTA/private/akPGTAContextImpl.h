
#pragma once

#include <stdint.h>

struct PGTABuffer;

class PGTAContextImpl
{
public:
    PGTAContextImpl();
    ~PGTAContextImpl();

    bool Initialize();
    void Shutdown();

    PGTABuffer* Update(const float delta, int32_t* numOutputBuffers);

    PGTABuffer* GetOutputBuffers(int32_t* numOutputBuffers);

private:
};
