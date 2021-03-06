#pragma once
#include "APIWrappers/CommandAllocator/CopyCommandAllocator.h"
#include "APIWrappers/CommandList/CopyCommandListImpl.h"
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Fence.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/ConstantBufferView.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "APIWrappers/RootSignature.h"
#include "Camera.h"
#include "Containers/Scene.h"
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

        bool Initialize(Device& device, DisplayController& displayController,
                        ResourceTracker& resourceTracker);
        void Unload();

        bool LoadScene(Device& device, SceneData& sceneData);
        void UnloadScene();
        Scene& GetScene();
        const Scene& GetScene() const;

        UINT GetBackbufferWidth() const;
        UINT GetBackbufferHeight() const;

        ResourceContainer& GetResourceContainer();

        CopyCommandListImpl& GetInitializationCommandList();
        bool FinishInitializationCommandList(Device& device);

        Camera& GetCamera();

        HWND GetHWND() const;

#ifdef BLK_RENDER_DEBUG
        Device& GetDevice();
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

} // namespace Boolka
