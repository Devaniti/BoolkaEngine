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

namespace Boolka
{

    class SceneData;
    class RenderEngineContext;

    class Scene
    {
    public:
        Scene();
        virtual ~Scene();

        enum SRVCount : UINT
        {
            MeshletSRVCount = 6,
            MaterialSRVCount = 1,
            RaytracingSRVCount = 2,
            RaytracingASCount = 1,
            SkyBoxSRVCount = 1,
            MaxSceneTextureCount = 512
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

        bool Initialize(Device& device, SceneData& sceneData, RenderEngineContext& engineContext);
        void Unload();

        // All opaque objects placed before all transparent objects
        // So objects in range [0, m_OpaqueObjectCount) - are opaque
        // And objects in range [m_OpaqueObjectCount, m_ObjectCount) - are
        // transparent
        UINT GetObjectCount() const;
        UINT GetOpaqueObjectCount() const;
        BatchManager& GetBatchManager();

    private:
        void InitializeBuffers(Device& device, const SceneData::SceneHeader& sceneHeader,
                               size_t& uploadSize, DescriptorHeap& mainSRVHeap,
                               UINT mainSRVHeapOffset);
        void PrecalculateSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                                size_t& uploadSize, size_t& lastTextureOffset);
        void PrecalculateTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                                  const SceneData::DataWrapper& dataWrapper, size_t& uploadSize,
                                  size_t& lastTextureOffset, std::vector<size_t>& textureOffsets);
        void PrecalculateAS(Device& device, GraphicCommandListImpl& initCommandList,
                            const SceneData::SceneHeader& sceneHeader,
                            const SceneData::DataWrapper& dataWrapper, Buffer& asBuildScratchBuffer,
                            Buffer& buildBuffer, UploadBuffer& tlasParametersUploadBuffer,
                            Buffer& tlasParametersBuffer, Buffer& postBuildDataBuffer,
                            ReadbackBuffer& postBuildDataReadbackBuffer,
                            std::vector<UINT64>& scratchBufferOffsets,
                            std::vector<UINT64>& buildOffsets);
        void InitializeSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                              DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset);
        void InitializeTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                                const SceneData::DataWrapper& dataWrapper,
                                const std::vector<size_t>& textureOffsets,
                                DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset);
        void UploadBuffers(GraphicCommandListImpl& initCommandList,
                           const SceneData::SceneHeader& sceneHeader, Buffer& uploadBuffer,
                           UINT64& uploadBufferOffset);
        void UploadSkyBox(GraphicCommandListImpl& initCommandList,
                          const SceneData::SceneHeader& sceneHeader, Buffer& uploadBuffer,
                          UINT64& uploadBufferOffset);
        void UploadTextures(GraphicCommandListImpl& initCommandList,
                            const SceneData::SceneHeader& sceneHeader,
                            const SceneData::DataWrapper& dataWrapper, Buffer& uploadBuffer,
                            UINT64& uploadBufferOffset);
        void BuildAS(GraphicCommandListImpl& initCommandList, Device& device,
                     RenderEngineContext& engineContext, const SceneData::SceneHeader& sceneHeader,
                     const SceneData::DataWrapper& dataWrapper, Buffer& asBuildScratchBuffer,
                     Buffer& buildBuffer, UploadBuffer& tlasParametersUploadBuffer,
                     Buffer& tlasParametersBuffer, Buffer& postBuildDataBuffer,
                     ReadbackBuffer& postBuildDataReadbackBuffer,
                     std::vector<UINT64>& scratchBufferOffsets, std::vector<UINT64>& buildOffsets,
                     DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset);

        UINT m_ObjectCount;
        UINT m_OpaqueObjectCount;
        Buffer m_VertexBuffer1;
        Buffer m_VertexBuffer2;
        Buffer m_VertexIndirectionBuffer;
        Buffer m_IndexBuffer;
        Buffer m_MeshletBuffer;
        Buffer m_ObjectBuffer;
        Buffer m_MaterialsBuffer;
        Buffer m_RTIndexBuffer;
        Buffer m_RTObjectIndexOffsetBuffer;
        Buffer m_ASBuffer;
        ResourceHeap m_ResourceHeap;
        BatchManager m_BatchManager;
        Texture2D m_SkyBoxCubemap;
        std::vector<Texture2D> m_SceneTextures;

        static const DXGI_FORMAT ms_SkyBoxTextureFormat;
        static const DXGI_FORMAT ms_SceneTexturesFormat;
    };

} // namespace Boolka
