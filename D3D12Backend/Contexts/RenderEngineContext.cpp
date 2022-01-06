#include "stdafx.h"

#include "RenderEngineContext.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"
#include "Contexts/RenderFrameContext.h"
#include "WindowManagement/DisplayController.h"

namespace Boolka
{

    RenderEngineContext::RenderEngineContext()
        : m_backbufferWidth(0)
        , m_backbufferHeight(0)
        , m_HWND(0)
#ifdef BLK_RENDER_DEBUG
        , m_Device(nullptr)
#endif
    {
    }

    RenderEngineContext::~RenderEngineContext()
    {
        BLK_ASSERT(m_backbufferWidth == 0);
        BLK_ASSERT(m_backbufferHeight == 0);
        BLK_ASSERT(m_HWND == 0);
    }

    bool RenderEngineContext::Initialize(Device& device, DisplayController& displayController,
                                         ResourceTracker& resourceTracker)
    {
        const WindowState& windowState = displayController.GetWindowState();

        m_backbufferWidth = windowState.width;
        m_backbufferHeight = windowState.height;

        bool res = m_InitializationCommandAllocator.Initialize(device);
        BLK_ASSERT_VAR(res);

        res = m_InitializationCommandList.Initialize(device, m_InitializationCommandAllocator.Get(),
                                                     nullptr);
        BLK_ASSERT_VAR(res);

        m_InitializationCommandList->Close();

        res = m_InitializationFence.Initialize(device);
        BLK_ASSERT_VAR(res);

        // TODO rewrite hardcoded value
        // Temp initial camera position for san-miguel scene
        res =
            m_Camera.Initialize(2.66f, -0.12f, BLK_DEG_TO_RAD(30.0f), {15.61f, 1.42f, 1.92f, 0.0f});
        BLK_ASSERT_VAR(res);

        m_HWND = displayController.GetHWND();

        res = m_resourceContainer.Initialize(device, *this, displayController, resourceTracker);
        BLK_ASSERT_VAR(res);

        ResetInitializationCommandList();
        res = m_timestampContainer.Initialize(device, m_InitializationCommandList);
        BLK_ASSERT_VAR(res);
        ExecuteInitializationCommandList(device);

#ifdef BLK_RENDER_DEBUG
        m_Device = &device;
#endif

        return true;
    }

    void RenderEngineContext::Unload()
    {
        m_InitializationCommandAllocator.Unload();
        m_InitializationCommandList.Unload();
        m_InitializationFence.Unload();

        m_resourceContainer.Unload();
        m_timestampContainer.Unload();

        UnloadScene();

        m_backbufferWidth = 0;
        m_backbufferHeight = 0;

        m_Camera.Unload();
        m_HWND = 0;
    }

    bool RenderEngineContext::LoadScene(Device& device, SceneData& sceneData)
    {
        return m_Scene.Initialize(device, sceneData, *this);
    }

    void RenderEngineContext::UnloadScene()
    {
        m_Scene.Unload();
    }

    const Scene& RenderEngineContext::GetScene() const
    {
        return m_Scene;
    }

    Scene& RenderEngineContext::GetScene()
    {
        return m_Scene;
    }

    void RenderEngineContext::BindSceneResourcesGraphic(CommandList& commandList)
    {
        DescriptorHeap& mainDescriptorHeap =
            m_resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        ID3D12DescriptorHeap* descriptorHeaps[] = {mainDescriptorHeap.Get()};
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::MainDescriptorTable),
            mainDescriptorHeap.GetGPUHandle(0));
    }

    void RenderEngineContext::BindSceneResourcesCompute(CommandList& commandList)
    {
        DescriptorHeap& mainDescriptorHeap =
            m_resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        ID3D12DescriptorHeap* descriptorHeaps[] = {mainDescriptorHeap.Get()};
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetComputeRootDescriptorTable(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::MainDescriptorTable),
            mainDescriptorHeap.GetGPUHandle(0));
    }

    UINT RenderEngineContext::GetBackbufferWidth() const
    {
        return m_backbufferWidth;
    }

    UINT RenderEngineContext::GetBackbufferHeight() const
    {
        return m_backbufferHeight;
    }

    ResourceContainer& RenderEngineContext::GetResourceContainer()
    {
        return m_resourceContainer;
    }

    TimestampContainer& RenderEngineContext::GetTimestampContainer()
    {
        return m_timestampContainer;
    }

    GraphicCommandListImpl& RenderEngineContext::GetInitializationCommandList()
    {
        return m_InitializationCommandList;
    }

    void RenderEngineContext::ResetInitializationCommandList()
    {
        m_InitializationCommandAllocator.Reset();
        m_InitializationCommandAllocator.ResetCommandList(m_InitializationCommandList, nullptr);
    }

    void RenderEngineContext::ExecuteInitializationCommandList(Device& device)
    {
        DebugProfileTimer commandListFlushWait;
        commandListFlushWait.Start();
        m_InitializationCommandList->Close();
        auto& commandQueue = device.GetGraphicQueue();
        commandQueue.ExecuteCommandList(m_InitializationCommandList);
        commandQueue.Flush();
        commandListFlushWait.Stop(L"Initialization command list execute");
    }

    Camera& RenderEngineContext::GetCamera()
    {
        return m_Camera;
    }

    HWND RenderEngineContext::GetHWND() const
    {
        return m_HWND;
    }

#ifdef BLK_RENDER_DEBUG
    Device& RenderEngineContext::GetDevice()
    {
        BLK_ASSERT(m_Device);
        return *m_Device;
    }

#endif

} // namespace Boolka
