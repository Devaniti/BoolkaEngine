#pragma once
#include "APIWrappers/RootSignature.h"
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "Containers/Scene.h"
#include "APIWrappers/CommandAllocator/CopyCommandAllocator.h"
#include "APIWrappers/CommandList/CopyCommandListImpl.h"
#include "APIWrappers/Fence.h"
#include "APIWrappers/Resources/Buffers/Views/ConstantBufferView.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "Camera.h"
#include "RenderSchedule/ResourceContainer.h"

namespace Boolka
{

    class Device;
    class DisplayController;
    class Texture2D;
    class CopyCommandList;
    class RenderFrameContext;

    class RenderEngineContext
    {
    public:
        RenderEngineContext();
        ~RenderEngineContext();

        bool Initialize(Device& device, DisplayController& displayController, ResourceTracker& resourceTracker);
        void Unload();

        bool LoadScene(Device& device, SceneData& sceneData);
        void UnloadScene();
        Scene& GetScene() { return m_Scene; }
        const Scene& GetScene() const { return m_Scene; }

        UINT GetBackbufferWidth() const { return m_backbufferWidth; }
        UINT GetBackbufferHeight() const { return m_backbufferHeight; }

        ResourceContainer& GetResourceContainer() { return m_resourceContainer; }

        CopyCommandListImpl& GetInitializationCommandList();
        bool FinishInitializationCommandList(Device& device);

        Camera& GetCamera() { return m_Camera; };

        HWND GetHWND() { return m_HWND; };

#ifdef BLK_RENDER_DEBUG
        Device& GetDevice() { BLK_ASSERT(m_Device); return *m_Device; };
#endif
    private:
        ResourceContainer m_resourceContainer;
        UINT m_backbufferWidth;
        UINT m_backbufferHeight;
        CopyCommandAllocator m_InitializationCommandAllocator;
        CopyCommandListImpl m_InitializationCommandList;
        Fence m_InitializationFence;
        Camera m_Camera;
        Scene m_Scene;
        HWND m_HWND;
#ifdef BLK_RENDER_DEBUG
        Device* m_Device;
#endif
    };

}
