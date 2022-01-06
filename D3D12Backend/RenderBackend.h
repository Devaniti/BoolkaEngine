#pragma once

namespace Boolka
{

    class SceneData;
    struct RenderCacheContainer;

    class [[nodiscard]] RenderBackend
    {
    public:
        virtual ~RenderBackend() = default;

        virtual bool Initialize(RenderCacheContainer& renderCache) = 0;
        virtual void Unload() = 0;
        virtual bool Present() = 0;
        virtual bool RenderFrame() = 0;

        [[nodiscard]] static RenderBackend* CreateRenderBackend();
        static void DeleteRenderBackend(RenderBackend* object);
        virtual bool LoadScene(SceneData& sceneData) = 0;
        virtual void UnloadScene() = 0;
    };

} // namespace Boolka
