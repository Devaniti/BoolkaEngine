#include "stdafx.h"
#include "RenderEngineContext.h"
#include "WindowManagement/DisplayController.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "APIWrappers/Device.h"

namespace Boolka
{

    RenderEngineContext::RenderEngineContext()
        : m_backbufferWidth(0)
        , m_backbufferHeight(0)
        , m_BackBuffers{}
        , m_BackBufferRTVs{}
    {
    }

    RenderEngineContext::~RenderEngineContext()
    {
        BLK_ASSERT(m_backbufferWidth == 0);
        BLK_ASSERT(m_backbufferHeight == 0);
        for (const auto& backbuffer : m_BackBuffers)
        {
            BLK_ASSERT(backbuffer == nullptr);
        }
    }

    bool RenderEngineContext::Initialize(Device& device, DisplayController& displayController)
    {
        const WindowState& windowState = displayController.GetWindowState();

        m_backbufferWidth = windowState.width;
        m_backbufferHeight = windowState.height;

        bool res = m_DefaultRootSig.Initialize(device, "RootSig.cso");
        BLK_ASSERT(res);
        res = m_BackBufferRTVDescriptorHeap.Initialize(device, BLK_IN_FLIGHT_FRAMES, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
        BLK_ASSERT(res);

        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            m_BackBuffers[i] = &displayController.GetBuffer(i);
            m_BackBufferRTVs[i].Initialize(device, *m_BackBuffers[i], DXGI_FORMAT_R8G8B8A8_UNORM, m_BackBufferRTVDescriptorHeap.GetCPUHandle(i));
        }

        res = m_DSVDescriptorHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
        BLK_ASSERT(res);
        res = m_MainDescriptorHeap.Initialize(device, BLK_IN_FLIGHT_FRAMES, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        BLK_ASSERT(res);

        res = m_InitializationCommandAllocator.Initialize(device);
        BLK_ASSERT(res);
        res = m_InitializationCommandList.Initialize(device, m_InitializationCommandAllocator.Get(), nullptr);
        BLK_ASSERT(res);
        m_InitializationCommandList->Close();
        res = m_InitializationFence.Initialize(device);
        BLK_ASSERT(res);

        return true;
    }

    void RenderEngineContext::Unload()
    {
        m_InitializationCommandAllocator.Unload();
        m_InitializationCommandList.Unload();
        m_InitializationFence.Unload();

        UnloadScene();

        m_DefaultRootSig.Unload();
        m_BackBufferRTVDescriptorHeap.Unload();
        m_DSVDescriptorHeap.Unload();
        m_MainDescriptorHeap.Unload();
        UNLOAD_ARRAY(m_BackBufferRTVs);

        m_backbufferWidth = 0;
        m_backbufferHeight = 0;
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            m_BackBuffers[i] = nullptr;
        }
    }

    bool RenderEngineContext::LoadScene(Device& device, SceneData& sceneData)
    {
        return m_Scene.Initialize(device, sceneData, *this);
    }

    void RenderEngineContext::UnloadScene()
    {
        m_Scene.Unload();
    }

    Texture2D& RenderEngineContext::GetSwapchainBackBuffer(UINT index)
    {
        BLK_ASSERT(index < BLK_IN_FLIGHT_FRAMES);
        BLK_ASSERT(m_BackBuffers[index] != nullptr);
        return *m_BackBuffers[index];
    }

    RenderTargetView& RenderEngineContext::GetSwapchainRenderTargetView(UINT index)
    {
        BLK_ASSERT(index < BLK_IN_FLIGHT_FRAMES);
        return m_BackBufferRTVs[index];
    }

    CopyCommandListImpl& RenderEngineContext::GetInitializationCommandList()
    {
        m_InitializationCommandAllocator.Reset();
        m_InitializationCommandAllocator.ResetCommandList(m_InitializationCommandList, nullptr);
        return m_InitializationCommandList;
    }

    bool RenderEngineContext::FinishInitializationCommandList(Device& device)
    {
        m_InitializationCommandList->Close();
        device.GetCopyQueue().ExecuteCommandList(m_InitializationCommandList);
        UINT64 expectedValue = m_InitializationFence.SignalGPU(device.GetCopyQueue());
        m_InitializationFence.WaitCPU(expectedValue);
        return true;
    }

}
