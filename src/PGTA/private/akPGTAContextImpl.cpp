
#include <private/akPGTAContextImpl.h>
#include <vector>
#include <memory>

PGTAContextImpl::PGTAContextImpl()
{
}

PGTAContextImpl::~PGTAContextImpl()
{
    Shutdown();
}

bool PGTAContextImpl::Initialize(const PGTAConfig& config)
{
    m_config = config;
    //m_scheduler = std::make_unique<PGTAScheduler>();

    /*if (!m_scheduler->Initialize(config))
    {
        return false;
    }*/
    return true;
}

void PGTAContextImpl::Shutdown()
{
}

void PGTAContextImpl::BindTrack(const PGTATrack* track)
{
    /*if (!m_scheduler)
    {
        return;
    }

    m_scheduler->SetPrimaryTrack(track);*/
}

PGTABuffer* PGTAContextImpl::Update(const float delta)
{
    return nullptr;
    //return m_scheduler->Update(delta);
}

PGTABuffer* PGTAContextImpl::GetOutputBuffer() const
{
    return nullptr;
}
