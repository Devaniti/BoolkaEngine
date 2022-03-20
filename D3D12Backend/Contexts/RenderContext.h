#pragma once

namespace Boolka
{

    class RenderEngineContext;
    class RenderFrameContext;
    class RenderThreadContext;

    class [[nodiscard]] RenderContext
    {
    public:
        RenderContext();
        ~RenderContext();

        bool Initialize(RenderEngineContext& engineContext, RenderFrameContext& frameContext,
                        RenderThreadContext& threadContext);
        void Unload();

        [[nodiscard]] RenderEngineContext& GetRenderEngineContext();
        [[nodiscard]] RenderFrameContext& GetRenderFrameContext();
        [[nodiscard]] RenderThreadContext& GetRenderThreadContext();

        [[nodiscard]] const RenderEngineContext& GetRenderEngineContext() const;
        [[nodiscard]] const RenderFrameContext& GetRenderFrameContext() const;
        [[nodiscard]] const RenderThreadContext& GetRenderThreadContext() const;

        [[nodiscard]] std::tuple<RenderEngineContext&, RenderFrameContext&, RenderThreadContext&>
        GetContexts();
        [[nodiscard]] std::tuple<const RenderEngineContext&, const RenderFrameContext&,
                                 const RenderThreadContext&>
        GetContexts() const;

    private:
        RenderEngineContext* m_EngineContext;
        RenderFrameContext* m_FrameContext;
        RenderThreadContext* m_ThreadContext;
    };

} // namespace Boolka
