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

        bool Initialize(RenderEngineContext& engineContext, RenderFrameContext& frameContext,
                        RenderThreadContext& threadContext);
        void Unload();

        RenderEngineContext& GetRenderEngineContext();
        RenderFrameContext& GetRenderFrameContext();
        RenderThreadContext& GetRenderThreadContext();

        const RenderEngineContext& GetRenderEngineContext() const;
        const RenderFrameContext& GetRenderFrameContext() const;
        const RenderThreadContext& GetRenderThreadContext() const;

        std::tuple<RenderEngineContext&, RenderFrameContext&, RenderThreadContext&> GetContexts();
        std::tuple<const RenderEngineContext&, const RenderFrameContext&,
                   const RenderThreadContext&>
        GetContexts() const;

    private:
        RenderEngineContext* m_engineContext;
        RenderFrameContext* m_frameContext;
        RenderThreadContext* m_threadContext;
    };

} // namespace Boolka
