#pragma once

namespace Boolka
{

    class RenderEngineContext;
    class RenderFrameContext;
    class RenderThreadContext;

    class RenderContext
    {
    public:
        RenderContext();
        ~RenderContext();

        bool Initialize(RenderEngineContext& engineContext, RenderFrameContext& frameContext, RenderThreadContext& threadContext);
        void Unload();

        RenderEngineContext& GetRenderEngineContext() { BLK_ASSERT(m_engineContext != nullptr); return *m_engineContext; };
        RenderFrameContext& GetRenderFrameContext() { BLK_ASSERT(m_frameContext != nullptr); return *m_frameContext; };
        RenderThreadContext& GetRenderThreadContext() { BLK_ASSERT(m_threadContext != nullptr); return *m_threadContext; };

        const RenderEngineContext& GetRenderEngineContext() const { BLK_ASSERT(m_engineContext != nullptr); return *m_engineContext; };
        const RenderFrameContext& GetRenderFrameContext() const { BLK_ASSERT(m_frameContext != nullptr); return *m_frameContext; };
        const RenderThreadContext& GetRenderThreadContext() const { BLK_ASSERT(m_threadContext != nullptr); return *m_threadContext; };

        std::tuple<RenderEngineContext&, RenderFrameContext&, RenderThreadContext&> GetContexts() { return { GetRenderEngineContext(), GetRenderFrameContext(), GetRenderThreadContext() }; };
        std::tuple<const RenderEngineContext&, const RenderFrameContext&, const RenderThreadContext&> GetContexts() const { return { GetRenderEngineContext(), GetRenderFrameContext(), GetRenderThreadContext() }; };

    private:
        RenderEngineContext* m_engineContext;
        RenderFrameContext* m_frameContext;
        RenderThreadContext* m_threadContext;
    };

}
