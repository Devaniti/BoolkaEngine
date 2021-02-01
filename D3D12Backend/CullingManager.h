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

        bool Initialize(UINT objectCount);
        void Unload();

        bool Cull(RenderFrameContext& frameContext, Scene& scene);
        bool Render(Scene& scene, CommandList& commandList);

    private:
        std::vector<bool> m_visibility;
    };

}
