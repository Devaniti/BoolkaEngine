#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/ResourceHeap.h"
#include "APIWrappers/Resources/Buffers/CommandSignature.h"
#include "APIWrappers/Resources/Buffers/ReadbackBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/IndexBufferView.h"
#include "APIWrappers/Resources/Buffers/Views/VertexBufferView.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/ShaderResourceView.h"
#include "BatchManager.h"
#include "Containers/Streaming/SceneData.h"
#include "HLSLShared.h"
#include "RTASContainer.h"
#include "Streaming/SceneDataReader.h"

namespace Boolka
{

    class RenderEngineContext;

    class [[nodiscard]] Scene
    {
    public:
        Scene();
        virtual ~Scene();

        enum SRVCount : UINT
        {
            MeshletSRVCount = 7,
            MaterialSRVCount = 1,
            RaytracingSRVCount = 2,
            RaytracingASCount = 1,
            SkyBoxSRVCount = 1
        };

        enum Limits : UINT
        {
            MaxSceneTextureCount = BLK_MAX_SCENE_TEXTURE_COUNT,
            MaxObjectCount = 2048,
            MaxMeshlets = 262144
        };

        enum SRVOffset : UINT
        {
            MeshletSRVOffset = 0,
            MaterialSRVOffset = MeshletSRVOffset + MeshletSRVCount,
            RaytracingSRVOffset = MaterialSRVOffset + MaterialSRVCount,
            RaytracingASOffset = RaytracingSRVOffset + RaytracingSRVCount,
            SkyBoxSRVOffset = RaytracingASOffset + RaytracingASCount,
            SceneSRVOffset = SkyBoxSRVOffset + SkyBoxSRVCount,
            MaxSize = SceneSRVOffset + MaxSceneTextureCount
        };

        bool Initialize(Device& device, const wchar_t* folderPath,
                        RenderEngineContext& engineContext);
        void Unload();

        void FinishLoading(Device& device, RenderEngineContext& engineContext);

        void FinishInitialization(Device& device);

        // All opaque objects placed before all transparent objects
        // So objects in range [0, m_OpaqueObjectCount) - are opaque
        // And objects in range [m_OpaqueObjectCount, m_ObjectCount) - are
        // transparent
        [[nodiscard]] UINT GetObjectCount() const;
        [[nodiscard]] UINT GetOpaqueObjectCount() const;
        [[nodiscard]] BatchManager& GetBatchManager();

    private:
        void InitializeBuffers(Device& device, const SceneData::SceneHeader& sceneHeader,
                               DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset);
        void PrecalculateSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                                size_t& lastTextureOffset);
        void PrecalculateTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                                  const SceneDataReader::HeaderWrapper& headerWrapper,
                                  size_t& lastTextureOffset, std::vector<size_t>& textureOffsets);
        void InitializeSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                              DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset);
        void InitializeTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                                const SceneDataReader::HeaderWrapper& headerWrapper,
                                const std::vector<size_t>& textureOffsets,
                                DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset);
        void UploadBuffers(Device& device, const SceneData::SceneHeader& sceneHeader,
                           UINT64& sourceOffset);
        void UploadSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                          UINT64& sourceOffset);
        void UploadTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                            const SceneDataReader::HeaderWrapper& headerWrapper,
                            UINT64& sourceOffset);

        UINT m_ObjectCount;
        UINT m_OpaqueObjectCount;
        Buffer m_VertexBuffer1;
        Buffer m_VertexBuffer2;
        Buffer m_VertexIndirectionBuffer;
        Buffer m_IndexBuffer;
        Buffer m_MeshletBuffer;
        Buffer m_MeshletCullBuffer;
        Buffer m_ObjectBuffer;
        Buffer m_MaterialsBuffer;
        Buffer m_RTIndexBuffer;
        Buffer m_RTObjectIndexOffsetBuffer;
        ResourceHeap m_ResourceHeap;
        BatchManager m_BatchManager;
        Texture2D m_SkyBoxCubemap;
        std::vector<Texture2D> m_SceneTextures;

        RTASContainer m_RTASContainer;
        SceneDataReader m_DataReader;

        static const DXGI_FORMAT ms_SkyBoxTextureFormat;
        static const DXGI_FORMAT ms_SceneTexturesFormat;
    };

} // namespace Boolka
