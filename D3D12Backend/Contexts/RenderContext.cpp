#include "stdafx.h"
#include "RenderContext.h"


namespace Boolka
{

    RenderContext::RenderContext()
        : m_engineContext(nullptr)
        , m_frameContext(nullptr)
        , m_threadContext(nullptr)
    {
    }

    RenderContext::~RenderContext()
    {
        BLK_ASSERT(m_engineContext == nullptr);
        BLK_ASSERT(m_frameContext == nullptr);
        BLK_ASSERT(m_threadContext == nullptr);
    }

    bool RenderContext::Initialize(RenderEngineContext& engineContext, RenderFrameContext& frameContext, RenderThreadContext& threadContext)
    {
        m_engineContext = &engineContext;
        m_frameContext = &frameContext;
        m_threadContext = &threadContext;

        return true;
    }

    void RenderContext::Unload()
    {
        m_engineContext = nullptr;
        m_frameContext = nullptr;
        m_threadContext = nullptr;
    }

}
