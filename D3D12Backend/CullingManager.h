#pragma once

namespace Boolka
{

    class Scene;
    class CommandList;
    class RenderEngineContext;
    class RenderFrameContext;
    class RenderThreadContext;

    class CullingManager
    {
    public:
        CullingManager() = default;
        ~CullingManager();

        bool Initialize(const Scene& scene);
        void Unload();

        bool Cull(const RenderFrameContext& frameContext, Scene& scene);
        const std::vector<bool>& GetVisibility() const { return m_visibility; };

    private:
        std::vector<bool> m_visibility;
    };

}
