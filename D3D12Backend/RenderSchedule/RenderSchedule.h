#pragma once

#include "RenderSchedule/ResourceTracker.h"
#include "RenderPasses/GBufferRenderPass.h"
#include "RenderPasses/PresentPass.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderThreadContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderEngineContext.h"

namespace Boolka
{

    class Device;
    class DisplayController; 
    class SceneData;

    class RenderSchedule
    {
    public:
        RenderSchedule() = default;
        ~RenderSchedule() = default;

        bool Initialize(Device& device, DisplayController& displayController);
        void Unload();

        bool LoadScene(Device& device, SceneData& sceneData);
        void UnloadScene();

        bool Render(Device& device, UINT frameIndex);

        ResourceTracker& GetResourceTracker() { return m_ResourceTracker; };

    private:
        bool PrepareFrame();
        bool RenderFrame(Device& device);

        void PrepareCommandList(GraphicCommandListImpl& commandList);
        void FinishCommandList(Device& device, GraphicCommandListImpl& commandList);

        RenderEngineContext m_EngineContext;
        RenderFrameContext m_FrameContext;
        RenderThreadContext m_ThreadContext;
        RenderContext m_RenderContext;
        ResourceTracker m_ResourceTracker;
        GBufferRenderPass m_DebugPass;
        PresentPass m_PresentPass;
    public:
    };

}
