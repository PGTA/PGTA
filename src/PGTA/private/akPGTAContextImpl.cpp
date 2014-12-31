
#include <private/akPGTAContextImpl.h>

PGTAContextImpl::PGTAContextImpl()
{
}

PGTAContextImpl::~PGTAContextImpl()
{
    Shutdown();
}

bool PGTAContextImpl::Initialize()
{
    return true;
}

void PGTAContextImpl::Shutdown()
{
}

PGTABuffer* PGTAContextImpl::Update(const float delta, int* numOutputBuffers)
{
    return nullptr;
}

PGTABuffer* PGTAContextImpl::GetOutputBuffers(int32_t* numOutputBuffers)
{
    return nullptr;
}
