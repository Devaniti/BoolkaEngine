#include "stdafx.h"

#include "RenderEngineContext.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
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
        BLK_ASSERT(res);
        res = m_InitializationCommandList.Initialize(device, m_InitializationCommandAllocator.Get(),
                                                     nullptr);
        BLK_ASSERT(res);
        m_InitializationCommandList->Close();
        res = m_InitializationFence.Initialize(device);
        BLK_ASSERT(res);

        // Temp initial camera position for san-miguel scene
        res = m_Camera.Initialize(0.0f, 0.0f, {6.35f, 3.46f, 1.22f, 0.0f});
        BLK_ASSERT(res);

        m_HWND = displayController.GetHWND();

        res = m_resourceContainer.Initialize(device, *this, displayController, resourceTracker);
        BLK_ASSERT(res);

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
