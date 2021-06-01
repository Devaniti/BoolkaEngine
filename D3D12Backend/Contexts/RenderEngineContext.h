#pragma once
#include "APIWrappers/CommandAllocator/GraphicCommandAllocator.h"
#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Fence.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/ConstantBufferView.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "APIWrappers/RootSignature.h"
#include "Camera.h"
#include "Containers/Scene.h"

namespace Boolka
{

    class Device;
    class DisplayController;
    class Texture2D;
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
        void BindSceneResourcesGraphic(CommandList& commandList);
        void BindSceneResourcesCompute(CommandList& commandList);

        UINT GetBackbufferWidth() const;
        UINT GetBackbufferHeight() const;

        ResourceContainer& GetResourceContainer();

        GraphicCommandListImpl& GetInitializationCommandList();
        void ResetInitializationCommandList();
        void ExecuteInitializationCommandList(Device& device);

        Camera& GetCamera();

        HWND GetHWND() const;

#ifdef BLK_RENDER_DEBUG
        Device& GetDevice();
#endif
    private:
        ResourceContainer m_resourceContainer;
        UINT m_backbufferWidth;
        UINT m_backbufferHeight;
        GraphicCommandAllocator m_InitializationCommandAllocator;
        GraphicCommandListImpl m_InitializationCommandList;
        Fence m_InitializationFence;
        Camera m_Camera;
        Scene m_Scene;
        HWND m_HWND;
#ifdef BLK_RENDER_DEBUG
        Device* m_Device;
#endif
    };

} // namespace Boolka
