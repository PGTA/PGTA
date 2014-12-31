
#ifndef AK_PGTA_CPP_H
#error "donut include pls"
#endif

namespace PGTA
{
    PGTAContext::PGTAContext(HPGTAContext context):
        m_pgtaContext(nullptr)
    {
    }

    PGTAContext::PGTAContext(const PGTAContext& other):
        m_pgtaContext(other.m_pgtaContext)
    {
    }

    PGTAContext::~PGTAContext()
    {
    }

    PGTABuffer* PGTAContext::Update(const float delta, int32_t* numOutputBuffers)
    {
        return pgtaUpdate(m_pgtaContext, delta, numOutputBuffers);
    }

    PGTABuffer* PGTAContext::GetOutputBuffers(int32_t* numOutputBuffers)
    {
        return pgtaGetOutputBuffers(m_pgtaContext, numOutputBuffers);
    }
}
