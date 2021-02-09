#pragma once

#include "RenderSchedule/ResourceTracker.h"
#include "RenderPasses/GBufferRenderPass.h"
#include "RenderPasses/PresentPass.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderThreadContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderEngineContext.h"
#include "RenderPasses/ZRenderPass.h"
#include "RenderPasses/UpdateRenderPass.h"
#include "RenderPasses/TransparentRenderPass.h"

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

        bool Render(Device& device, UINT frameIndex);

        ResourceTracker& GetResourceTracker() { return m_ResourceTracker; };

    private:
        bool InitializeRenderPasses(Device& device);
        void UnloadRenderPasses();

        bool PrepareFrame();
        bool RenderFrame(Device& device);

        void PrepareCommandList(GraphicCommandListImpl& commandList);
        void FinishCommandList(Device& device, GraphicCommandListImpl& commandList);

        RenderEngineContext m_EngineContext;
        RenderFrameContext m_FrameContext;
        RenderThreadContext m_ThreadContext;
        RenderContext m_RenderContext;
        ResourceTracker m_ResourceTracker;
        UpdateRenderPass m_UpdatePass;
        ZRenderPass m_ZPass;
        GBufferRenderPass m_GbufferPass;
        TransparentRenderPass m_TransparentPass;
        PresentPass m_PresentPass;
    public:
    };

}
