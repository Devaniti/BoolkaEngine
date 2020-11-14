#pragma once

namespace Boolka
{
    
    class SceneData;

    class RenderBackend
    {
    public:
        virtual ~RenderBackend() {};

        virtual bool Initialize() = 0;
        virtual void Unload() = 0;
        virtual bool Present() = 0;
        virtual bool RenderFrame() = 0;

        static RenderBackend* CreateRenderBackend();
        static void DeleteRenderBackend(RenderBackend* object);
        virtual bool LoadScene(SceneData& sceneData) = 0;
        virtual void UnloadScene() = 0;
    };

}
