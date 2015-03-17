
#ifndef AK_PGTA_CPP_H
#error "donut include pls"
#endif

namespace PGTA
{
    PGTAContext::PGTAContext(HPGTAContext context):
        m_pgtaContext(context)
    {
    }

    PGTAContext::PGTAContext(const PGTAContext& other):
        m_pgtaContext(other.m_pgtaContext)
    {
    }

    PGTAContext::~PGTAContext()
    {
    }

    PGTABuffer PGTAContext::Update(const float deltaSeconds)
    {
        return pgtaUpdate(m_pgtaContext, deltaSeconds);
    }

    PGTABuffer PGTAContext::GetOutputBuffer()
    {
        return pgtaGetOutputBuffer(m_pgtaContext);
    }

    void PGTAContext::BindTrack(HPGTATrack track)
    {
        return pgtaBindTrack(m_pgtaContext, track);
    }
}
