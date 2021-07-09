#include "stdafx.h"

#include "Scene.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/Raytracing/AccelerationStructure/BottomLevelAS.h"
#include "APIWrappers/Raytracing/AccelerationStructure/TopLevelAS.h"
#include "APIWrappers/RenderDebug.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/ResourceTransition.h"
#include "BoolkaCommon/DebugHelpers/DebugOutputStream.h"
#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"
#include "Contexts/RenderEngineContext.h"

namespace Boolka
{

    const DXGI_FORMAT Scene::ms_SkyBoxTextureFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
    const DXGI_FORMAT Scene::ms_SceneTexturesFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    Scene::Scene()
        : m_ObjectCount(0)
        , m_OpaqueObjectCount(0)
    {
    }

    Scene::~Scene()
    {
        BLK_ASSERT(m_ObjectCount == 0);
        BLK_ASSERT(m_OpaqueObjectCount == 0);
    }

    bool Scene::Initialize(Device& device, SceneData& sceneData, RenderEngineContext& engineContext)
    {
        BLK_ASSERT(m_ObjectCount == 0);
        BLK_ASSERT(m_OpaqueObjectCount == 0);

        ResourceContainer& resourceContainer = engineContext.GetResourceContainer();
        DescriptorHeap& mainSRVHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        UINT mainSRVHeapOffset =
            static_cast<UINT>(ResourceContainer::MainSRVDescriptorHeapOffsets::SceneSRVHeapOffset);

        const auto dataWrapper = sceneData.GetSceneWrapper();
        const auto sceneHeader = dataWrapper.header;

        BLK_ASSERT(sceneHeader.textureCount < MaxSceneTextureCount);

        m_ObjectCount = sceneHeader.objectCount;
        m_OpaqueObjectCount = sceneHeader.opaqueCount;

        m_BatchManager.Initialize(device, *this, engineContext);

        size_t uploadSize = 0;
        InitializeBuffers(device, sceneHeader, uploadSize, mainSRVHeap, mainSRVHeapOffset);

        size_t lastTextureOffset = 0;
        PrecalculateSkyBox(device, sceneHeader, uploadSize, lastTextureOffset);

        sceneData.PrepareTextureHeaders();

        std::vector<size_t> textureOffsets(sceneHeader.textureCount);
        PrecalculateTextures(device, sceneHeader, dataWrapper, uploadSize, lastTextureOffset,
                             textureOffsets);

        size_t heapSize = lastTextureOffset;
        m_ResourceHeap.Initialize(device, heapSize, D3D12_HEAP_TYPE_DEFAULT,
                                  D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES);

        InitializeSkyBox(device, sceneHeader, mainSRVHeap, mainSRVHeapOffset);
        InitializeTextures(device, sceneHeader, dataWrapper, textureOffsets, mainSRVHeap,
                           mainSRVHeapOffset);

        UploadBuffer uploadBuffer;
        uploadBuffer.Initialize(device, BLK_CEIL_TO_POWER_OF_TWO(uploadSize, 64));

        sceneData.PrepareCPUObjectHeaders();

        engineContext.ResetInitializationCommandList();
        auto& initCommandList = engineContext.GetInitializationCommandList();

        Buffer asBuildScratchBuffer;
        Buffer buildBuffer;
        UploadBuffer tlasParametersUploadBuffer;
        Buffer tlasParametersBuffer;
        Buffer postBuildDataBuffer;
        ReadbackBuffer postBuildDataReadbackBuffer;
        std::vector<UINT64> scratchBufferOffsets(sceneHeader.opaqueCount);
        std::vector<UINT64> buildOffsets(sceneHeader.opaqueCount);
        PrecalculateAS(device, initCommandList, sceneHeader, dataWrapper, asBuildScratchBuffer,
                       buildBuffer, tlasParametersUploadBuffer, tlasParametersBuffer,
                       postBuildDataBuffer, postBuildDataReadbackBuffer, scratchBufferOffsets,
                       buildOffsets);

        DebugProfileTimer streamingWait;
        streamingWait.Start();
        sceneData.PrepareBinaryData();
        streamingWait.Stop(L"Streaming wait");

        uploadBuffer.Upload(dataWrapper.binaryData, uploadSize);

        UINT64 uploadBufferOffset = 0;

        UploadBuffers(initCommandList, sceneHeader, uploadBuffer, uploadBufferOffset);
        UploadSkyBox(initCommandList, sceneHeader, uploadBuffer, uploadBufferOffset);
        UploadTextures(initCommandList, sceneHeader, dataWrapper, uploadBuffer, uploadBufferOffset);

        BLK_ASSERT(uploadBufferOffset == uploadSize);

        BuildAS(initCommandList, device, engineContext, sceneHeader, dataWrapper,
                asBuildScratchBuffer, buildBuffer, tlasParametersUploadBuffer, tlasParametersBuffer,
                postBuildDataBuffer, postBuildDataReadbackBuffer, scratchBufferOffsets,
                buildOffsets, mainSRVHeap, mainSRVHeapOffset);

        engineContext.ExecuteInitializationCommandList(device);

        uploadBuffer.Unload();

        asBuildScratchBuffer.Unload();
        buildBuffer.Unload();
        tlasParametersUploadBuffer.Unload();
        tlasParametersBuffer.Unload();
        postBuildDataBuffer.Unload();
        postBuildDataReadbackBuffer.Unload();

        return true;
    }

    void Scene::Unload()
    {
        BLK_UNLOAD_ARRAY(m_SceneTextures);
        m_SceneTextures.clear();

        m_SkyBoxCubemap.Unload();

        m_ResourceHeap.Unload();

        m_VertexBuffer1.Unload();
        m_VertexBuffer2.Unload();
        m_VertexIndirectionBuffer.Unload();
        m_IndexBuffer.Unload();
        m_MeshletBuffer.Unload();
        m_MeshletCullBuffer.Unload();
        m_ObjectBuffer.Unload();
        m_MaterialsBuffer.Unload();
        m_RTIndexBuffer.Unload();
        m_RTObjectIndexOffsetBuffer.Unload();
        m_ASBuffer.Unload();

        m_ObjectCount = 0;
        m_OpaqueObjectCount = 0;
        m_BatchManager.Unload();
    }

    UINT Scene::GetObjectCount() const
    {
        return m_ObjectCount;
    }

    UINT Scene::GetOpaqueObjectCount() const
    {
        return m_OpaqueObjectCount;
    }

    BatchManager& Scene::GetBatchManager()
    {
        return m_BatchManager;
    }

    void Scene::InitializeBuffers(Device& device, const SceneData::SceneHeader& sceneHeader,
                                  size_t& uploadSize, DescriptorHeap& mainSRVHeap,
                                  UINT mainSRVHeapOffset)
    {
        bool res;
        res = m_VertexBuffer1.Initialize(device, sceneHeader.vertex1Size, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.vertex1Size;
        RenderDebug::SetDebugName(m_VertexBuffer1.Get(), L"Scene::m_VertexBuffer1");

        res = m_VertexBuffer2.Initialize(device, sceneHeader.vertex2Size, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.vertex2Size;
        RenderDebug::SetDebugName(m_VertexBuffer2.Get(), L"Scene::m_VertexBuffer2");

        res = m_VertexIndirectionBuffer.Initialize(
            device, sceneHeader.vertexIndirectionSize, D3D12_HEAP_TYPE_DEFAULT,
            D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.vertexIndirectionSize;
        RenderDebug::SetDebugName(m_VertexIndirectionBuffer.Get(),
                                  L"Scene::m_VertexIndirectionBuffer");

        res = m_IndexBuffer.Initialize(device, sceneHeader.indexSize, D3D12_HEAP_TYPE_DEFAULT,
                                       D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.indexSize;
        RenderDebug::SetDebugName(m_IndexBuffer.Get(), L"Scene::m_IndexBuffer");

        res = m_MeshletBuffer.Initialize(device, sceneHeader.meshletsSize, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.meshletsSize;
        RenderDebug::SetDebugName(m_MeshletBuffer.Get(), L"Scene::m_MeshletBuffer");

        res = m_MeshletCullBuffer.Initialize(device, sceneHeader.meshletsCullSize,
                                             D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE,
                                             D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.meshletsCullSize;
        RenderDebug::SetDebugName(m_MeshletCullBuffer.Get(), L"Scene::m_MeshletCullBuffer");

        res = m_ObjectBuffer.Initialize(device, sceneHeader.objectsSize, D3D12_HEAP_TYPE_DEFAULT,
                                        D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.objectsSize;
        RenderDebug::SetDebugName(m_ObjectBuffer.Get(), L"Scene::m_ObjectBuffer");

        res =
            m_MaterialsBuffer.Initialize(device, sceneHeader.materialsSize, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.materialsSize;
        RenderDebug::SetDebugName(m_MaterialsBuffer.Get(), L"Scene::m_MaterialsBuffer");

        res =
            m_RTIndexBuffer.Initialize(device, sceneHeader.rtIndiciesSize, D3D12_HEAP_TYPE_DEFAULT,
                                       D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.rtIndiciesSize;
        RenderDebug::SetDebugName(m_RTIndexBuffer.Get(), L"Scene::m_RTIndexBuffer");

        res = m_RTObjectIndexOffsetBuffer.Initialize(
            device, sceneHeader.rtObjectIndexOffsetSize, D3D12_HEAP_TYPE_DEFAULT,
            D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        uploadSize += sceneHeader.rtObjectIndexOffsetSize;
        RenderDebug::SetDebugName(m_RTIndexBuffer.Get(), L"Scene::m_RTObjectIndexOffsetBuffer");

        UINT srvSlot = MeshletSRVOffset;
        ShaderResourceView::Initialize(device, m_VertexBuffer1,
                                       sceneHeader.vertex1Size / sizeof(HLSLShared::VertexData1),
                                       sizeof(HLSLShared::VertexData1),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        ShaderResourceView::Initialize(device, m_VertexBuffer2,
                                       sceneHeader.vertex1Size / sizeof(HLSLShared::VertexData1),
                                       sizeof(HLSLShared::VertexData1),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        ShaderResourceView::Initialize(
            device, m_VertexIndirectionBuffer, sceneHeader.vertexIndirectionSize / sizeof(uint32_t),
            sizeof(uint32_t), mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        ShaderResourceView::Initialize(device, m_IndexBuffer,
                                       sceneHeader.indexSize / sizeof(uint32_t), sizeof(uint32_t),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        ShaderResourceView::Initialize(device, m_MeshletBuffer,
                                       sceneHeader.meshletsSize / sizeof(HLSLShared::MeshletData),
                                       sizeof(HLSLShared::MeshletData),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        ShaderResourceView::Initialize(device, m_MeshletCullBuffer,
                                       sceneHeader.meshletsCullSize /
                                           sizeof(HLSLShared::MeshletCullData),
                                       sizeof(HLSLShared::MeshletCullData),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        ShaderResourceView::Initialize(device, m_ObjectBuffer,
                                       sceneHeader.objectsSize / sizeof(HLSLShared::ObjectData),
                                       sizeof(HLSLShared::ObjectData),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        BLK_ASSERT(srvSlot == MaterialSRVOffset);

        ShaderResourceView::Initialize(device, m_MaterialsBuffer,
                                       sceneHeader.materialsSize / sizeof(HLSLShared::MaterialData),
                                       sizeof(HLSLShared::MaterialData),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        BLK_ASSERT(srvSlot == RaytracingSRVOffset);

        ShaderResourceView::Initialize(
            device, m_RTIndexBuffer, sceneHeader.rtIndiciesSize / sizeof(uint32_t),
            sizeof(uint32_t), mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        ShaderResourceView::Initialize(device, m_RTObjectIndexOffsetBuffer,
                                       sceneHeader.rtObjectIndexOffsetSize / sizeof(uint32_t),
                                       sizeof(uint32_t),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        BLK_ASSERT(srvSlot == RaytracingASOffset);
    }

    void Scene::PrecalculateSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                                   size_t& uploadSize, size_t& lastTextureOffset)
    {
        UINT skyBoxResolution = sceneHeader.skyBoxResolution;
        UINT skyBoxMipCount = sceneHeader.skyBoxMipCount;

        size_t alignment;
        size_t size;
        Texture2D::GetRequiredSize(alignment, size, device, skyBoxResolution, skyBoxResolution,
                                   skyBoxMipCount, ms_SkyBoxTextureFormat, D3D12_RESOURCE_FLAG_NONE,
                                   BLK_TEXCUBE_FACE_COUNT);

        lastTextureOffset += size;

        uploadSize += Texture2D::GetUploadSize(skyBoxResolution, skyBoxResolution, skyBoxMipCount,
                                               ms_SkyBoxTextureFormat, D3D12_RESOURCE_FLAG_NONE,
                                               BLK_TEXCUBE_FACE_COUNT);
    }

    void Scene::PrecalculateTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                                     const SceneData::DataWrapper& dataWrapper, size_t& uploadSize,
                                     size_t& lastTextureOffset, std::vector<size_t>& textureOffsets)
    {
        m_SceneTextures.resize(sceneHeader.textureCount);
        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            const auto& textureHeader = dataWrapper.textureHeaders[i];
            UINT width = textureHeader.width;
            UINT height = textureHeader.height;
            UINT mipCount = textureHeader.mipCount;
            BLK_ASSERT(width != 0);
            BLK_ASSERT(height != 0);

            size_t alignment;
            size_t size;
            Texture2D::GetRequiredSize(alignment, size, device, textureHeader.width,
                                       textureHeader.height, mipCount, ms_SceneTexturesFormat,
                                       D3D12_RESOURCE_FLAG_NONE);

            lastTextureOffset = BLK_CEIL_TO_POWER_OF_TWO(lastTextureOffset, alignment);
            textureOffsets[i] = lastTextureOffset;
            lastTextureOffset += size;

            uploadSize += Texture2D::GetUploadSize(width, height, mipCount, ms_SceneTexturesFormat,
                                                   D3D12_RESOURCE_FLAG_NONE);
        }
    }

    void Scene::PrecalculateAS(Device& device, GraphicCommandListImpl& initCommandList,
                               const SceneData::SceneHeader& sceneHeader,
                               const SceneData::DataWrapper& dataWrapper,
                               Buffer& asBuildScratchBuffer, Buffer& buildBuffer,
                               UploadBuffer& tlasParametersUploadBuffer,
                               Buffer& tlasParametersBuffer, Buffer& postBuildDataBuffer,
                               ReadbackBuffer& postBuildDataReadbackBuffer,
                               std::vector<UINT64>& scratchBufferOffsets,
                               std::vector<UINT64>& buildOffsets)
    {
        BLK_GPU_SCOPE(initCommandList.Get(), "Scene::PrecalculateAS");

        const auto* objects = dataWrapper.cpuObjectHeaders;
        const UINT objectCount = sceneHeader.opaqueCount;

        UINT64 scratchSize = 0;
        UINT64 asSize = 0;
        for (size_t i = 0; i < objectCount; ++i)
        {
            UINT vertexSize = 16;
            UINT64 currentScratchSize = 0;
            UINT64 currentASSize = 0;
            BottomLevelAS::GetSizes(device, sceneHeader.vertex1Size / vertexSize, vertexSize,
                                    objects[i].rtIndexCount, currentScratchSize, currentASSize);
            scratchBufferOffsets[i] = scratchSize;
            buildOffsets[i] = asSize;
            scratchSize += currentScratchSize;
            asSize += currentASSize;
        }

        UINT64 tlasScratchSize;
        UINT64 tlasSize;
        TopLevelAS::GetSizes(device, objectCount, tlasScratchSize, tlasSize);
        scratchSize = std::max(scratchSize, tlasScratchSize);

        UINT64 tlasParametersSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * objectCount;

        tlasParametersUploadBuffer.Initialize(device, tlasParametersSize);
        tlasParametersBuffer.Initialize(device, tlasParametersSize, D3D12_HEAP_TYPE_DEFAULT,
                                        D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
        asBuildScratchBuffer.Initialize(device, scratchSize, D3D12_HEAP_TYPE_DEFAULT,
                                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        buildBuffer.Initialize(device, asSize, D3D12_HEAP_TYPE_DEFAULT,
                               D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                               D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

        g_WDebugOutput << "Pre-Compaction BLAS total size: " << asSize / 1024.0f / 1024.0f << "MB"
                       << std::endl;

        size_t postBuildInfoSize =
            sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC) *
            objectCount;
        postBuildDataBuffer.Initialize(device, postBuildInfoSize, D3D12_HEAP_TYPE_DEFAULT,
                                       D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                       D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        postBuildDataReadbackBuffer.Initialize(device, postBuildInfoSize);
    }

    void Scene::InitializeSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                                 DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset)
    {
        UINT skyBoxResolution = sceneHeader.skyBoxResolution;
        UINT skyBoxMipCount = sceneHeader.skyBoxMipCount;
        m_SkyBoxCubemap.Initialize(device, m_ResourceHeap, 0, skyBoxResolution, skyBoxResolution,
                                   skyBoxMipCount, ms_SkyBoxTextureFormat, D3D12_RESOURCE_FLAG_NONE,
                                   nullptr, D3D12_RESOURCE_STATE_COMMON, BLK_TEXCUBE_FACE_COUNT);
        RenderDebug::SetDebugName(m_SkyBoxCubemap.Get(), L"Scene::m_SkyBoxCubemap");

        ShaderResourceView::InitializeCube(
            device, m_SkyBoxCubemap, mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + SkyBoxSRVOffset),
            ms_SkyBoxTextureFormat);
    }

    void Scene::InitializeTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                                   const SceneData::DataWrapper& dataWrapper,
                                   const std::vector<size_t>& textureOffsets,
                                   DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset)
    {
        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            auto& texture = m_SceneTextures[i];
            const auto& textureHeader = dataWrapper.textureHeaders[i];

            texture.Initialize(device, m_ResourceHeap, textureOffsets[i], textureHeader.width,
                               textureHeader.height, textureHeader.mipCount, ms_SceneTexturesFormat,
                               D3D12_RESOURCE_FLAG_NONE, nullptr, D3D12_RESOURCE_STATE_COMMON);

            RenderDebug::SetDebugName(texture.Get(), L"Scene::m_SceneTextures[%d]", i);
        }

        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            ShaderResourceView::Initialize(
                device, m_SceneTextures[i],
                mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + SceneSRVOffset + i));
        }

        for (UINT i = sceneHeader.textureCount; i < MaxSceneTextureCount; ++i)
        {
            ShaderResourceView::InitializeNullDescriptorTexture2D(
                device, ms_SceneTexturesFormat,
                mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + SceneSRVOffset + i));
        }
    }

    void Scene::UploadSkyBox(GraphicCommandListImpl& initCommandList,
                             const SceneData::SceneHeader& sceneHeader, Buffer& uploadBuffer,
                             UINT64& uploadBufferOffset)
    {
        BLK_GPU_SCOPE(initCommandList.Get(), "Scene::UploadSkyBox");
        UINT skyBoxResolution = sceneHeader.skyBoxResolution;
        UINT skyBoxMipCount = sceneHeader.skyBoxMipCount;
        UINT bytesPerPixel = Texture2D::GetBPP(ms_SkyBoxTextureFormat) / 8;
        BLK_ASSERT(bytesPerPixel != 0);

        BLK_ASSERT(bytesPerPixel == sizeof(Vector4));

        UINT16 subresource = 0;

        for (UINT face = 0; face < BLK_TEXCUBE_FACE_COUNT; ++face)
        {
            UINT resolution = skyBoxResolution;
            for (UINT16 mipNumber = 0; mipNumber < skyBoxMipCount; ++mipNumber)
            {
                BLK_ASSERT(resolution != 0);

                size_t rowPitch = BLK_CEIL_TO_POWER_OF_TWO(resolution * bytesPerPixel,
                                                           D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
                size_t textureSize = BLK_CEIL_TO_POWER_OF_TWO(
                    rowPitch * resolution, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

                D3D12_TEXTURE_COPY_LOCATION copyDest;
                copyDest.pResource = m_SkyBoxCubemap.Get();
                copyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                copyDest.SubresourceIndex = face * skyBoxMipCount + mipNumber;

                D3D12_TEXTURE_COPY_LOCATION copySource;
                copySource.pResource = uploadBuffer.Get();
                copySource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                copySource.PlacedFootprint.Offset = uploadBufferOffset;
                copySource.PlacedFootprint.Footprint.Format = ms_SkyBoxTextureFormat;
                copySource.PlacedFootprint.Footprint.Width = resolution;
                copySource.PlacedFootprint.Footprint.Height = resolution;
                copySource.PlacedFootprint.Footprint.Depth = 1;
                copySource.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(rowPitch);

                initCommandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySource, nullptr);

                resolution >>= 1;
                uploadBufferOffset += textureSize;
            }
        }

        ResourceTransition::Transition(initCommandList, m_SkyBoxCubemap,
                                       D3D12_RESOURCE_STATE_COPY_DEST,
                                       D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                           D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    }

    void Scene::UploadTextures(GraphicCommandListImpl& initCommandList,
                               const SceneData::SceneHeader& sceneHeader,
                               const SceneData::DataWrapper& dataWrapper, Buffer& uploadBuffer,
                               UINT64& uploadBufferOffset)
    {
        BLK_GPU_SCOPE(initCommandList.Get(), "Scene::UploadTextures");
        UINT bytesPerPixel = Texture2D::GetBPP(ms_SceneTexturesFormat) / 8;
        BLK_ASSERT(bytesPerPixel != 0);

        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            auto& texture = m_SceneTextures[i];
            auto& textureHeader = dataWrapper.textureHeaders[i];

            UINT width = textureHeader.width;
            UINT height = textureHeader.height;
            UINT16 mipCount = textureHeader.mipCount;
            for (UINT16 mipNumber = 0; mipNumber < mipCount; ++mipNumber)
            {
                BLK_ASSERT(width != 0);
                BLK_ASSERT(height != 0);

                size_t rowPitch = BLK_CEIL_TO_POWER_OF_TWO(width * bytesPerPixel,
                                                           D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
                size_t textureSize = BLK_CEIL_TO_POWER_OF_TWO(
                    rowPitch * height, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

                D3D12_TEXTURE_COPY_LOCATION copyDest;
                copyDest.pResource = texture.Get();
                copyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                copyDest.SubresourceIndex = mipNumber;

                D3D12_TEXTURE_COPY_LOCATION copySource;
                copySource.pResource = uploadBuffer.Get();
                copySource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                copySource.PlacedFootprint.Offset = uploadBufferOffset;
                copySource.PlacedFootprint.Footprint.Format = ms_SceneTexturesFormat;
                copySource.PlacedFootprint.Footprint.Width = width;
                copySource.PlacedFootprint.Footprint.Height = height;
                copySource.PlacedFootprint.Footprint.Depth = 1;
                copySource.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(rowPitch);

                initCommandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySource, nullptr);

                width >>= 1;
                height >>= 1;
                uploadBufferOffset += textureSize;
            }
        }

        std::vector<Resource*> resources(sceneHeader.textureCount);
        for (size_t i = 0; i < sceneHeader.textureCount; ++i)
        {
            resources[i] = &m_SceneTextures[i];
        }
        ResourceTransition::TransitionMany(initCommandList, resources.data(),
                                           D3D12_RESOURCE_STATE_COPY_DEST,
                                           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                               D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
                                           sceneHeader.textureCount);
    }

    void Scene::BuildAS(
        GraphicCommandListImpl& initCommandList, Device& device, RenderEngineContext& engineContext,
        const SceneData::SceneHeader& sceneHeader, const SceneData::DataWrapper& dataWrapper,
        Buffer& asBuildScratchBuffer, Buffer& buildBuffer, UploadBuffer& tlasParametersUploadBuffer,
        Buffer& tlasParametersBuffer, Buffer& postBuildDataBuffer,
        ReadbackBuffer& postBuildDataReadbackBuffer, std::vector<UINT64>& scratchBufferOffsets,
        std::vector<UINT64>& buildOffsets, DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset)
    {
        const UINT objectCount = sceneHeader.opaqueCount;
        const auto* objects = dataWrapper.cpuObjectHeaders;
        UINT64 buildBufferAddress = buildBuffer->GetGPUVirtualAddress();
        UINT64 scratchBufferAddress = asBuildScratchBuffer->GetGPUVirtualAddress();
        UINT64 vertexBufferAddress = m_VertexBuffer1->GetGPUVirtualAddress();
        UINT vertexSize = 16;
        UINT64 indexBufferAddress = m_RTIndexBuffer->GetGPUVirtualAddress();
        UINT64 postBuildDataAddress = postBuildDataBuffer->GetGPUVirtualAddress();
        {
            BLK_GPU_SCOPE(initCommandList.Get(), "Scene::BuildAS");
            {
                BLK_GPU_SCOPE(initCommandList.Get(), "BuildBLAS");
                for (size_t i = 0; i < objectCount; ++i)
                {
                    UINT64 postBuildDataOffset =
                        sizeof(
                            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC) *
                        i;
                    BottomLevelAS::Initialize(
                        initCommandList, buildBufferAddress + buildOffsets[i],
                        scratchBufferAddress + scratchBufferOffsets[i], vertexBufferAddress,
                        sceneHeader.vertex1Size / vertexSize, vertexSize,
                        indexBufferAddress + objects[i].rtIndexOffset * sizeof(uint32_t),
                        objects[i].rtIndexCount, postBuildDataAddress + postBuildDataOffset);
                }
            }

            ResourceTransition::Transition(initCommandList, postBuildDataBuffer,
                                           D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                           D3D12_RESOURCE_STATE_COPY_SOURCE);
            size_t postBuildInfoSize =
                sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC) *
                objectCount;
            initCommandList->CopyResource(postBuildDataReadbackBuffer.Get(),
                                          postBuildDataBuffer.Get());
        }

        engineContext.ExecuteInitializationCommandList(device);
        engineContext.ResetInitializationCommandList();

        BLK_GPU_SCOPE(initCommandList.Get(), "Scene::BuildAS");

        void* postBuildData = postBuildDataReadbackBuffer.Map(
            0, sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC) *
                   objectCount);
        auto* postBuildDescs = static_cast<
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC*>(
            postBuildData);

        UINT64 asFinalSize = 0;

        for (size_t i = 0; i < objectCount; ++i)
        {
            asFinalSize +=
                BLK_CEIL_TO_POWER_OF_TWO(postBuildDescs[i].CompactedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        }

        g_WDebugOutput << "Post-Compaction BLAS total size: " << asFinalSize / 1024.0f / 1024.0f
                       << "MB" << std::endl;

        UINT64 scratchSize;
        UINT64 tlasSize;
        TopLevelAS::GetSizes(device, objectCount, scratchSize, tlasSize);
        asFinalSize += tlasSize;

        g_WDebugOutput << "TLAS size: " << tlasSize / 1024.0f / 1024.0f << "MB" << std::endl;

        m_ASBuffer.Initialize(device, asFinalSize, D3D12_HEAP_TYPE_DEFAULT,
                              D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                              D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
        RenderDebug::SetDebugName(m_ASBuffer.Get(), L"Scene::m_ASBuffer");

        UINT64 asDestAddress = m_ASBuffer->GetGPUVirtualAddress();
        UINT64 blasDestAddress = asDestAddress + tlasSize;

        UINT64 currentBlasDestAddress = blasDestAddress;
        {
            BLK_GPU_SCOPE(initCommandList.Get(), "CopyAndCompactBLAS");
            for (size_t i = 0; i < objectCount; ++i)
            {
                initCommandList->CopyRaytracingAccelerationStructure(
                    currentBlasDestAddress, buildBufferAddress + buildOffsets[i],
                    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_COMPACT);
                currentBlasDestAddress += BLK_CEIL_TO_POWER_OF_TWO(
                    postBuildDescs[i].CompactedSizeInBytes,
                    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
            }
        }

        currentBlasDestAddress = blasDestAddress;

        {
            void* tlasParametersData = tlasParametersUploadBuffer.Map();
            auto* tlasParameters = static_cast<D3D12_RAYTRACING_INSTANCE_DESC*>(tlasParametersData);

            for (size_t i = 0; i < objectCount; ++i)
            {
                D3D12_RAYTRACING_INSTANCE_DESC& param = tlasParameters[i];
                // Identity transform matrix
                param = {};
                param.Transform[0][0] = 1.0f;
                param.Transform[1][1] = 1.0f;
                param.Transform[2][2] = 1.0f;

                param.InstanceID = objects[i].materialIndex; // Material index
                param.InstanceMask = 1;
                param.InstanceContributionToHitGroupIndex = 0;
                param.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
                param.AccelerationStructure = currentBlasDestAddress;
                currentBlasDestAddress += BLK_CEIL_TO_POWER_OF_TWO(
                    postBuildDescs[i].CompactedSizeInBytes,
                    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
            }
            tlasParametersUploadBuffer.Unmap();
        }

        postBuildDataReadbackBuffer.Unmap();

        initCommandList->CopyResource(tlasParametersBuffer.Get(), tlasParametersUploadBuffer.Get());
        ResourceTransition::Transition(initCommandList, tlasParametersBuffer,
                                       D3D12_RESOURCE_STATE_COPY_DEST,
                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

        UINT64 tlasParametersAddress = tlasParametersBuffer->GetGPUVirtualAddress();

        TopLevelAS::Initialize(initCommandList, asDestAddress, scratchBufferAddress,
                               tlasParametersAddress, objectCount);

        ShaderResourceView::InitializeAccelerationStructure(
            device, asDestAddress,
            mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + RaytracingASOffset));
    }

    void Scene::UploadBuffers(GraphicCommandListImpl& initCommandList,
                              const SceneData::SceneHeader& sceneHeader, Buffer& uploadBuffer,
                              UINT64& uploadBufferOffset)
    {
        BLK_GPU_SCOPE(initCommandList.Get(), "Scene::UploadBuffers");
        initCommandList->CopyBufferRegion(m_VertexBuffer1.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.vertex1Size);
        uploadBufferOffset += sceneHeader.vertex1Size;

        initCommandList->CopyBufferRegion(m_VertexBuffer2.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.vertex2Size);
        uploadBufferOffset += sceneHeader.vertex2Size;

        initCommandList->CopyBufferRegion(m_VertexIndirectionBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.vertexIndirectionSize);
        uploadBufferOffset += sceneHeader.vertexIndirectionSize;

        initCommandList->CopyBufferRegion(m_IndexBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.indexSize);
        uploadBufferOffset += sceneHeader.indexSize;

        initCommandList->CopyBufferRegion(m_MeshletBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.meshletsSize);
        uploadBufferOffset += sceneHeader.meshletsSize;

        initCommandList->CopyBufferRegion(m_MeshletCullBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.meshletsCullSize);
        uploadBufferOffset += sceneHeader.meshletsCullSize;

        initCommandList->CopyBufferRegion(m_ObjectBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.objectsSize);
        uploadBufferOffset += sceneHeader.objectsSize;

        initCommandList->CopyBufferRegion(m_MaterialsBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.materialsSize);
        uploadBufferOffset += sceneHeader.materialsSize;

        initCommandList->CopyBufferRegion(m_RTIndexBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.rtIndiciesSize);
        uploadBufferOffset += sceneHeader.rtIndiciesSize;

        initCommandList->CopyBufferRegion(m_RTObjectIndexOffsetBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.rtObjectIndexOffsetSize);
        uploadBufferOffset += sceneHeader.rtObjectIndexOffsetSize;

        Resource* resources[] = {&m_VertexBuffer1,           &m_VertexBuffer2,
                                 &m_VertexIndirectionBuffer, &m_IndexBuffer,
                                 &m_MeshletBuffer,           &m_ObjectBuffer,
                                 &m_RTIndexBuffer,           &m_RTObjectIndexOffsetBuffer};
        ResourceTransition::TransitionMany(initCommandList, resources,
                                           D3D12_RESOURCE_STATE_COPY_DEST,
                                           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                               D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
                                           ARRAYSIZE(resources));
    }

} // namespace Boolka
