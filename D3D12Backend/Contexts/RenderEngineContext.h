#pragma once
#include "APIWrappers/RootSignature.h"
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "Containers/Scene.h"
#include "APIWrappers/CommandAllocator/CopyCommandAllocator.h"
#include "APIWrappers/CommandList/CopyCommandListImpl.h"
#include "APIWrappers/Fence.h"

namespace Boolka
{

    class Device;
    class DisplayController;
    class Texture2D;
    class CopyCommandList;

    class RenderEngineContext
    {
    public:
        RenderEngineContext();
        ~RenderEngineContext();

        bool Initialize(Device& device, DisplayController& displayController);
        void Unload();

        bool LoadScene(Device& device, const SceneData& sceneData);
        void UnloadScene();
        Scene& GetScene() { return m_Scene; };

        UINT GetBackbufferWidth() { return m_backbufferWidth; };
        UINT GetBackbufferHeight() { return m_backbufferHeight; };

        Texture2D& GetSwapchainBackBuffer(UINT index);
        RenderTargetView& GetSwapchainRenderTargetView(UINT index);

        RootSignature& GetDefaultRootSig() { return m_DefaultRootSig; }
        DescriptorHeap& GetDSVDescriptorHeap() { return m_DSVDescriptorHeap; }
        DescriptorHeap& GetMainDescriptorHeap() { return m_MainDescriptorHeap; }

        CopyCommandListImpl& GetInitializationCommandList();
        bool FinishInitializationCommandList(Device& device);

    private:
        UINT m_backbufferWidth;
        UINT m_backbufferHeight;
        Texture2D* m_BackBuffers[BLK_IN_FLIGHT_FRAMES];
        RenderTargetView m_BackBufferRTVs[BLK_IN_FLIGHT_FRAMES];
        RootSignature m_DefaultRootSig;
        DescriptorHeap m_BackBufferRTVDescriptorHeap;
        DescriptorHeap m_DSVDescriptorHeap;
        DescriptorHeap m_MainDescriptorHeap;
        CopyCommandAllocator m_InitializationCommandAllocator;
        CopyCommandListImpl m_InitializationCommandList;
        Fence m_InitializationFence;
        Scene m_Scene;
    };

}
