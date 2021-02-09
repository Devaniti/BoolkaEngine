#include "stdafx.h"
#include "RenderSchedule.h"
#include "APIWrappers/Device.h"
#include "Contexts/RenderContext.h"

namespace Boolka
{

    bool RenderSchedule::Initialize(Device& device, DisplayController& displayController)
    {
        m_ResourceTracker.Initialize(device, 10);

        bool res = m_EngineContext.Initialize(device, displayController);
        BLK_ASSERT(res);
        res = m_FrameContext.Initialize(device);
        BLK_ASSERT(res);
        res = m_ThreadContext.Initialize(device);
        BLK_ASSERT(res);

        m_RenderContext.Initialize(m_EngineContext, m_FrameContext, m_ThreadContext);

        InitializeRenderPasses(device);

        return true;
    }

    void RenderSchedule::Unload()
    {
        m_RenderContext.Unload();
        m_EngineContext.Unload();
        m_FrameContext.Unload();
        m_ThreadContext.Unload();

        UnloadRenderPasses();

        m_ResourceTracker.Unload();
    }

    bool RenderSchedule::Render(Device& device, UINT frameIndex)
    {
        m_FrameContext.FlipFrame(m_EngineContext, frameIndex);
        m_ThreadContext.FlipFrame(frameIndex);

        PrepareFrame();
        bool res = RenderFrame(device);
        BLK_ASSERT(res);

        return true;
    }

    bool RenderSchedule::LoadScene(Device& device, SceneData& sceneData)
    {
        bool res = m_EngineContext.LoadScene(device, sceneData);
        BLK_ASSERT(res);

        return true;
    }

    bool RenderSchedule::InitializeRenderPasses(Device& device)
    {
        bool res = m_UpdatePass.Initialize(device, m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);
        res = m_ZPass.Initialize(device, m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);
        res = m_GbufferPass.Initialize(device, m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);
        res = m_TransparentPass.Initialize(device, m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);
        res = m_PresentPass.Initialize(device, m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);

        return true;
    }

    void RenderSchedule::UnloadRenderPasses()
    {
        m_UpdatePass.Unload();
        m_ZPass.Unload();
        m_GbufferPass.Unload();
        m_TransparentPass.Unload();
        m_PresentPass.Unload();
    }

    bool RenderSchedule::PrepareFrame()
    {
        Scene& scene = m_RenderContext.GetRenderEngineContext().GetScene();
        scene.GetBatchManager().PrepareBatches(m_FrameContext, scene);
        return true;
    }

    bool RenderSchedule::RenderFrame(Device& device)
    {
        GraphicCommandListImpl& currentCommandList = m_ThreadContext.GetGraphicCommandList();

        PrepareCommandList(currentCommandList);

        bool res = m_UpdatePass.Render(m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);
        res = m_ZPass.Render(m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);
        res = m_GbufferPass.Render(m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);
        res = m_TransparentPass.Render(m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);
        res = m_PresentPass.Render(m_RenderContext, m_ResourceTracker);
        BLK_ASSERT(res);

        FinishCommandList(device, currentCommandList);

        return true;
    }

    void RenderSchedule::PrepareCommandList(GraphicCommandListImpl& commandList)
    {
        commandList->SetGraphicsRootSignature(m_EngineContext.GetDefaultRootSig().Get());
    }

    void RenderSchedule::FinishCommandList(Device& device, GraphicCommandListImpl& commandList)
    {
        commandList->Close();
        device.GetGraphicQueue().ExecuteCommandList(commandList);
    }

}
