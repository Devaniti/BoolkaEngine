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
#include "Containers/TimestampContainer.h"

namespace Boolka
{

    class Device;
    class DisplayController;
    class Texture2D;
    class RenderFrameContext;

    class [[nodiscard]] RenderEngineContext
    {
    public:
        RenderEngineContext();
        ~RenderEngineContext();

        bool Initialize(Device& device, DisplayController& displayController,
                        ResourceTracker& resourceTracker);
        void Unload();

        bool LoadScene(Device& device, SceneData& sceneData);
        void UnloadScene();
        [[nodiscard]] Scene& GetScene();
        [[nodiscard]] const Scene& GetScene() const;
        void BindSceneResourcesGraphic(CommandList& commandList);
        void BindSceneResourcesCompute(CommandList& commandList);

        [[nodiscard]] UINT GetBackbufferWidth() const;
        [[nodiscard]] UINT GetBackbufferHeight() const;

        [[nodiscard]] ResourceContainer& GetResourceContainer();
        [[nodiscard]] TimestampContainer& GetTimestampContainer();

        [[nodiscard]] GraphicCommandListImpl& GetInitializationCommandList();
        void ResetInitializationCommandList();
        void ExecuteInitializationCommandList(Device& device);

        [[nodiscard]] Camera& GetCamera();

        [[nodiscard]] HWND GetHWND() const;

#ifdef BLK_RENDER_DEBUG
        [[nodiscard]] Device& GetDevice();
#endif
    private:
        ResourceContainer m_resourceContainer;
        TimestampContainer m_timestampContainer;
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
