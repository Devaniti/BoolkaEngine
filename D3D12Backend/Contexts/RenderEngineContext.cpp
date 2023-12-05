#include "stdafx.h"

#include "RenderEngineContext.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "Contexts/RenderFrameContext.h"
#include "WindowManagement/DisplayController.h"

namespace Boolka
{

    RenderEngineContext::RenderEngineContext()
        : m_BackbufferWidth(0)
        , m_BackbufferHeight(0)
        , m_HWND(0)
#ifdef BLK_RENDER_DEBUG
        , m_Device(nullptr)
#endif
    {
    }

    RenderEngineContext::~RenderEngineContext()
    {
        BLK_ASSERT(m_BackbufferWidth == 0);
        BLK_ASSERT(m_BackbufferHeight == 0);
        BLK_ASSERT(m_HWND == 0);
    }

    bool RenderEngineContext::Initialize(Device& device, DisplayController& displayController,
                                         ResourceTracker& resourceTracker)
    {
        BLK_CPU_SCOPE("RenderEngineContext::Initialize");

        const WindowState& windowState = displayController.GetWindowState();

        m_BackbufferWidth = windowState.width;
        m_BackbufferHeight = windowState.height;

        bool res = m_InitializationCommandAllocator.Initialize(device);
        BLK_ASSERT_VAR(res);

        for (auto& cmdList : m_InitializationCommandList)
        {
            res = cmdList.Initialize(device, m_InitializationCommandAllocator.Get(), nullptr);
            BLK_ASSERT_VAR(res);
            cmdList->Close();
        }

        res = m_InitializationFence.Initialize(device);
        BLK_ASSERT_VAR(res);

        // TODO rewrite hardcoded value
        // Temp initial camera position for san-miguel scene
        res =
            m_Camera.Initialize(2.66f, -0.12f, BLK_DEG_TO_RAD(30.0f), {15.61f, 1.42f, 1.92f, 0.0f});
        BLK_ASSERT_VAR(res);

        m_HWND = displayController.GetHWND();

        res = m_ResourceContainer.Initialize(device, *this, displayController, resourceTracker);
        BLK_ASSERT_VAR(res);

        ResetInitializationCommandList();

        res = m_TimestampContainer.Initialize(device, GetInitializationCommandList());
        BLK_ASSERT_VAR(res);

#ifdef BLK_RENDER_DEBUG
        m_Device = &device;
#endif

        return true;
    }

    void RenderEngineContext::Unload()
    {
        m_InitializationCommandAllocator.Unload();
        for (auto& cmdList : m_InitializationCommandList)
        {
            cmdList.Unload();
        }
        m_InitializationFence.Unload();

        m_PSOContainer.Unload();
        m_ResourceContainer.Unload();
        m_TimestampContainer.Unload();

        UnloadScene();

        m_BackbufferWidth = 0;
        m_BackbufferHeight = 0;

        m_Camera.Unload();
        m_HWND = 0;
    }

    bool RenderEngineContext::StartSceneLoading(Device& device, const wchar_t* folderPath)
    {
        return m_Scene.Initialize(device, folderPath, *this);
    }

    void RenderEngineContext::FinishSceneLoading(Device& device, const wchar_t* folderPath)
    {
        m_Scene.FinishLoading(device, *this);
    }

    void RenderEngineContext::FinishInitialization(Device& device)
    {
        m_PSOContainer.FinishInitialization(device);
        m_Scene.FinishInitialization(device);
    }

    void RenderEngineContext::UnloadScene()
    {
        m_Scene.Unload();
    }

    void RenderEngineContext::BuildPSOs(Device& device)
    {
        bool res = m_PSOContainer.Initialize(device, *this);
        BLK_ASSERT_VAR(res);
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
            m_ResourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        ID3D12DescriptorHeap* descriptorHeaps[] = {mainDescriptorHeap.Get()};
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::MainDescriptorTable),
            mainDescriptorHeap.GetGPUHandle(0));
    }

    void RenderEngineContext::BindSceneResourcesCompute(CommandList& commandList)
    {
        DescriptorHeap& mainDescriptorHeap =
            m_ResourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        ID3D12DescriptorHeap* descriptorHeaps[] = {mainDescriptorHeap.Get()};
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetComputeRootDescriptorTable(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::MainDescriptorTable),
            mainDescriptorHeap.GetGPUHandle(0));
    }

    UINT RenderEngineContext::GetBackbufferWidth() const
    {
        return m_BackbufferWidth;
    }

    UINT RenderEngineContext::GetBackbufferHeight() const
    {
        return m_BackbufferHeight;
    }

    ResourceContainer& RenderEngineContext::GetResourceContainer()
    {
        return m_ResourceContainer;
    }

    TimestampContainer& RenderEngineContext::GetTimestampContainer()
    {
        return m_TimestampContainer;
    }

    PSOContainer& RenderEngineContext::GetPSOContainer()
    {
        return m_PSOContainer;
    }

    GraphicCommandListImpl& RenderEngineContext::GetInitializationCommandList()
    {
        return m_InitializationCommandList[0];
    }

    void RenderEngineContext::ResetInitializationCommandList()
    {
        m_InitializationCommandAllocator.ResetCommandList(m_InitializationCommandList[0], nullptr);
    }

    void RenderEngineContext::ExecuteInitializationCommandList(Device& device)
    {
        m_InitializationCommandList[0]->Close();
        auto& commandQueue = device.GetGraphicQueue();
        commandQueue.ExecuteCommandList(m_InitializationCommandList[0]);
        //++m_CurrentCommandListIndex;
    }

    void RenderEngineContext::FlushInitializationCommandList(Device& device)
    {
        ExecuteInitializationCommandList(device);
        device.GetGraphicQueue().Flush();
        ResetInitializationCommandList();
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
