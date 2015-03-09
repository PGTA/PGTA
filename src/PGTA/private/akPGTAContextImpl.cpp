
#include <private/akPGTAContextImpl.h>
#include <vector>

PGTAContextImpl::PGTAContextImpl()
{
}

PGTAContextImpl::~PGTAContextImpl()
{
    Shutdown();
}

bool PGTAContextImpl::Initialize(const PGTAConfig& config)
{
    return true;
}

void PGTAContextImpl::Shutdown()
{
}

void PGTAContextImpl::BindTrack(const PGTATrack* track)
{
    m_tracks.emplace_back(track);
}

PGTABuffer* PGTAContextImpl::Update(const float delta)
{
    return nullptr;
}

PGTABuffer* PGTAContextImpl::GetOutputBuffers() const
{
    return nullptr;
}
