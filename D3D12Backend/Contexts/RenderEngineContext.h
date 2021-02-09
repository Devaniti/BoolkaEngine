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

        bool Initialize(Device& device, DisplayController& displayController);
        void Unload();

        bool LoadScene(Device& device, SceneData& sceneData);
        void UnloadScene();
        Scene& GetScene() { return m_Scene; };
        const Scene& GetScene() const { return m_Scene; };

        UINT GetBackbufferWidth() const { return m_backbufferWidth; };
        UINT GetBackbufferHeight() const { return m_backbufferHeight; };

        Texture2D& GetSwapchainBackBuffer(UINT index);
        RenderTargetView& GetSwapchainRenderTargetView(UINT index);

        Texture2D& GetDepthBuffer() { return m_DepthBuffer; };
        DepthStencilView& GetDepthStencilView() { return m_DepthStencilView; };

        Buffer& GetConstantBuffer(UINT index);
        UploadBuffer& GetConstantUploadBuffer(UINT index);
        ConstantBufferView& GetConstantBufferView(UINT index);

        RootSignature& GetDefaultRootSig() { return m_DefaultRootSig; }
        DescriptorHeap& GetDSVDescriptorHeap() { return m_DSVDescriptorHeap; }
        DescriptorHeap& GetMainDescriptorHeap() { return m_MainDescriptorHeap; }

        CopyCommandListImpl& GetInitializationCommandList();
        bool FinishInitializationCommandList(Device& device);

        Camera& GetCamera() { return m_Camera; };

    private:
        UINT m_backbufferWidth;
        UINT m_backbufferHeight;
        Texture2D* m_BackBuffers[BLK_IN_FLIGHT_FRAMES];
        RenderTargetView m_BackBufferRTVs[BLK_IN_FLIGHT_FRAMES];
        Texture2D m_DepthBuffer;
        DepthStencilView m_DepthStencilView;
        Buffer m_ConstantBuffers[BLK_IN_FLIGHT_FRAMES];
        UploadBuffer m_ConstantUploadBuffers[BLK_IN_FLIGHT_FRAMES];
        ConstantBufferView m_ConstantBufferViews[BLK_IN_FLIGHT_FRAMES];
        RootSignature m_DefaultRootSig;
        DescriptorHeap m_BackBufferRTVDescriptorHeap;
        DescriptorHeap m_DSVDescriptorHeap;
        DescriptorHeap m_MainDescriptorHeap;
        CopyCommandAllocator m_InitializationCommandAllocator;
        CopyCommandListImpl m_InitializationCommandList;
        Fence m_InitializationFence;
        Camera m_Camera;
        Scene m_Scene;
    };

}
