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

    bool RenderContext::Initialize(RenderEngineContext& engineContext,
                                   RenderFrameContext& frameContext,
                                   RenderThreadContext& threadContext)
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

    const RenderFrameContext& RenderContext::GetRenderFrameContext() const
    {
        BLK_ASSERT(m_frameContext != nullptr);
        return *m_frameContext;
    }

    RenderFrameContext& RenderContext::GetRenderFrameContext()
    {
        BLK_ASSERT(m_frameContext != nullptr);
        return *m_frameContext;
    }

    const RenderThreadContext& RenderContext::GetRenderThreadContext() const
    {
        BLK_ASSERT(m_threadContext != nullptr);
        return *m_threadContext;
    }

    RenderThreadContext& RenderContext::GetRenderThreadContext()
    {
        BLK_ASSERT(m_threadContext != nullptr);
        return *m_threadContext;
    }

    const RenderEngineContext& RenderContext::GetRenderEngineContext() const
    {
        BLK_ASSERT(m_engineContext != nullptr);
        return *m_engineContext;
    }

    RenderEngineContext& RenderContext::GetRenderEngineContext()
    {
        BLK_ASSERT(m_engineContext != nullptr);
        return *m_engineContext;
    }

    std::tuple<const RenderEngineContext&, const RenderFrameContext&, const RenderThreadContext&>
    RenderContext::GetContexts() const
    {
        return {GetRenderEngineContext(), GetRenderFrameContext(), GetRenderThreadContext()};
    }

    std::tuple<RenderEngineContext&, RenderFrameContext&, RenderThreadContext&> RenderContext::
        GetContexts()
    {
        return {GetRenderEngineContext(), GetRenderFrameContext(), GetRenderThreadContext()};
    }

} // namespace Boolka
