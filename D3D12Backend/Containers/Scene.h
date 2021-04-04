#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/ResourceHeap.h"
#include "APIWrappers/Resources/Buffers/CommandSignature.h"
#include "APIWrappers/Resources/Buffers/Views/IndexBufferView.h"
#include "APIWrappers/Resources/Buffers/Views/VertexBufferView.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/ShaderResourceView.h"
#include "BatchManager.h"
#include "Containers/Streaming/SceneData.h"

namespace Boolka
{

    class SceneData;
    class RenderEngineContext;

    class Scene
    {
    public:

        Scene();
        virtual ~Scene();

        bool Initialize(Device& device, SceneData& sceneData, RenderEngineContext& engineContext);
        void Unload();

        // All opaque objects placed before all transparent objects
        // So objects in range [0, m_OpaqueObjectCount) - are opaque
        // And objects in range [m_OpaqueObjectCount, m_ObjectCount) - are
        // transparent
        UINT GetObjectCount() const;
        UINT GetOpaqueObjectCount() const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetMeshletsTable() const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetSkyBoxTable() const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetSceneTexturesTable() const;
        DescriptorHeap& GetSRVDescriptorHeap();
        BatchManager& GetBatchManager();

        void BindResources(CommandList& commandList);

    private:

        enum SRVCount : UINT
        {
            MeshletSRVCount = 6,
            SkyBoxSRVCount = 1
        };

        enum SRVOffset : UINT
        {
            MeshletSRVOffset = 0,
            SkyBoxSRVOffset = MeshletSRVOffset + MeshletSRVCount,
            SceneSRVOffset = SkyBoxSRVOffset + SkyBoxSRVCount
        };

        UINT m_ObjectCount;
        UINT m_OpaqueObjectCount;
        Buffer m_VertexBuffer1;
        Buffer m_VertexBuffer2;
        Buffer m_VertexIndirectionBuffer;
        Buffer m_IndexBuffer;
        Buffer m_MeshletBuffer;
        Buffer m_ObjectBuffer;
        DescriptorHeap m_SRVDescriptorHeap;
        ResourceHeap m_ResourceHeap;
        BatchManager m_BatchManager;
        Texture2D m_SkyBoxCubemap;
        std::vector<Texture2D> m_SceneTextures;

        static const DXGI_FORMAT ms_SkyBoxTextureFormat;
        static const DXGI_FORMAT ms_SceneTexturesFormat;
    };

} // namespace Boolka
