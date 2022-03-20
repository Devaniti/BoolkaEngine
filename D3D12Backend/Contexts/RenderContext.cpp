#include "stdafx.h"

#include "RenderContext.h"

namespace Boolka
{

    RenderContext::RenderContext()
        : m_EngineContext(nullptr)
        , m_FrameContext(nullptr)
        , m_ThreadContext(nullptr)
    {
    }

    RenderContext::~RenderContext()
    {
        BLK_ASSERT(m_EngineContext == nullptr);
        BLK_ASSERT(m_FrameContext == nullptr);
        BLK_ASSERT(m_ThreadContext == nullptr);
    }

    bool RenderContext::Initialize(RenderEngineContext& engineContext,
                                   RenderFrameContext& frameContext,
                                   RenderThreadContext& threadContext)
    {
        m_EngineContext = &engineContext;
        m_FrameContext = &frameContext;
        m_ThreadContext = &threadContext;

        return true;
    }

    void RenderContext::Unload()
    {
        m_EngineContext = nullptr;
        m_FrameContext = nullptr;
        m_ThreadContext = nullptr;
    }

    const RenderFrameContext& RenderContext::GetRenderFrameContext() const
    {
        BLK_ASSERT(m_FrameContext != nullptr);
        return *m_FrameContext;
    }

    RenderFrameContext& RenderContext::GetRenderFrameContext()
    {
        BLK_ASSERT(m_FrameContext != nullptr);
        return *m_FrameContext;
    }

    const RenderThreadContext& RenderContext::GetRenderThreadContext() const
    {
        BLK_ASSERT(m_ThreadContext != nullptr);
        return *m_ThreadContext;
    }

    RenderThreadContext& RenderContext::GetRenderThreadContext()
    {
        BLK_ASSERT(m_ThreadContext != nullptr);
        return *m_ThreadContext;
    }

    const RenderEngineContext& RenderContext::GetRenderEngineContext() const
    {
        BLK_ASSERT(m_EngineContext != nullptr);
        return *m_EngineContext;
    }

    RenderEngineContext& RenderContext::GetRenderEngineContext()
    {
        BLK_ASSERT(m_EngineContext != nullptr);
        return *m_EngineContext;
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
