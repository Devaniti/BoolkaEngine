#include "stdafx.h"

#include "Scene.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/RenderDebug.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
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

    bool Scene::Initialize(Device& device, const wchar_t* folderPath,
                           RenderEngineContext& engineContext)
    {
        BLK_ASSERT(m_ObjectCount == 0);
        BLK_ASSERT(m_OpaqueObjectCount == 0);

        BLK_CPU_SCOPE("Scene::Initialize");

        m_DataReader.OpenScene(device, folderPath);

        ResourceContainer& resourceContainer = engineContext.GetResourceContainer();
        DescriptorHeap& mainSRVHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        UINT mainSRVHeapOffset =
            static_cast<UINT>(ResourceContainer::MainSRVDescriptorHeapOffsets::SceneSRVHeapOffset);

        const SceneDataReader::HeaderWrapper headerWrapper = m_DataReader.GetHeaderWrapper();
        const SceneData::SceneHeader& sceneHeader = *headerWrapper.header;

        BLK_ASSERT(headerWrapper.header->textureCount < MaxSceneTextureCount);

        m_ObjectCount = headerWrapper.header->objectCount;
        m_OpaqueObjectCount = headerWrapper.header->opaqueCount;

        m_BatchManager.Initialize(device, *this, engineContext);

        InitializeBuffers(device, sceneHeader, mainSRVHeap, mainSRVHeapOffset);

        size_t lastTextureOffset = 0;
        PrecalculateSkyBox(device, sceneHeader, lastTextureOffset);

        std::vector<size_t> textureOffsets(headerWrapper.header->textureCount);
        PrecalculateTextures(device, sceneHeader, headerWrapper, lastTextureOffset, textureOffsets);

        size_t heapSize = lastTextureOffset;
        m_ResourceHeap.Initialize(device, heapSize, D3D12_HEAP_TYPE_DEFAULT,
                                  D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES);

        InitializeSkyBox(device, sceneHeader, mainSRVHeap, mainSRVHeapOffset);
        InitializeTextures(device, sceneHeader, headerWrapper, textureOffsets, mainSRVHeap,
                           mainSRVHeapOffset);

        MemoryBlock rtCacheheaderWrapper{};

        UINT64 sourceOffset = 0;

        UploadBuffers(device, sceneHeader, sourceOffset);

        m_RTASContainer.Initialize(device, engineContext, headerWrapper, m_VertexBuffer1,
                                   m_RTIndexBuffer);

        UploadSkyBox(device, sceneHeader, sourceOffset);
        UploadTextures(device, sceneHeader, headerWrapper, sourceOffset);

        return true;
    }

    void Scene::Unload()
    {
        m_DataReader.CloseReader();

        m_RTASContainer.Unload();

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

        m_ObjectCount = 0;
        m_OpaqueObjectCount = 0;
        m_BatchManager.Unload();
    }

    void Scene::FinishLoading(Device& device, RenderEngineContext& engineContext)
    {
        const SceneDataReader::HeaderWrapper headerWrapper = m_DataReader.GetHeaderWrapper();

        m_RTASContainer.FinishLoading(device, engineContext, headerWrapper);
    }

    void Scene::FinishInitialization()
    {
        m_RTASContainer.FinishInitialization();
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
                                  DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset)
    {
        BLK_CPU_SCOPE("Scene::InitializeBuffers");

        bool res;
        res = m_VertexBuffer1.Initialize(device, sceneHeader.vertex1Size, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_VertexBuffer1.Get(), L"Scene::m_VertexBuffer1");

        res = m_VertexBuffer2.Initialize(device, sceneHeader.vertex2Size, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_VertexBuffer2.Get(), L"Scene::m_VertexBuffer2");

        res = m_VertexIndirectionBuffer.Initialize(
            device, sceneHeader.vertexIndirectionSize, D3D12_HEAP_TYPE_DEFAULT,
            D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_VertexIndirectionBuffer.Get(),
                                  L"Scene::m_VertexIndirectionBuffer");

        res = m_IndexBuffer.Initialize(device, sceneHeader.indexSize, D3D12_HEAP_TYPE_DEFAULT,
                                       D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_IndexBuffer.Get(), L"Scene::m_IndexBuffer");

        res = m_MeshletBuffer.Initialize(device, sceneHeader.meshletsSize, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_MeshletBuffer.Get(), L"Scene::m_MeshletBuffer");

        res = m_MeshletCullBuffer.Initialize(device, sceneHeader.meshletsCullSize,
                                             D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE,
                                             D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_MeshletCullBuffer.Get(), L"Scene::m_MeshletCullBuffer");

        res = m_ObjectBuffer.Initialize(device, sceneHeader.objectsSize, D3D12_HEAP_TYPE_DEFAULT,
                                        D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_ObjectBuffer.Get(), L"Scene::m_ObjectBuffer");

        res =
            m_MaterialsBuffer.Initialize(device, sceneHeader.materialsSize, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_MaterialsBuffer.Get(), L"Scene::m_MaterialsBuffer");

        res =
            m_RTIndexBuffer.Initialize(device, sceneHeader.rtIndiciesSize, D3D12_HEAP_TYPE_DEFAULT,
                                       D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_RTIndexBuffer.Get(), L"Scene::m_RTIndexBuffer");

        res = m_RTObjectIndexOffsetBuffer.Initialize(
            device, sceneHeader.rtObjectIndexOffsetSize, D3D12_HEAP_TYPE_DEFAULT,
            D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_RTObjectIndexOffsetBuffer.Get(),
                                  L"Scene::m_RTObjectIndexOffsetBuffer");

        UINT srvSlot = MeshletSRVOffset;
        ShaderResourceView::Initialize(device, m_VertexBuffer1,
                                       sceneHeader.vertex1Size / sizeof(HLSLShared::VertexData1),
                                       sizeof(HLSLShared::VertexData1),
                                       mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + srvSlot++));

        ShaderResourceView::Initialize(device, m_VertexBuffer2,
                                       sceneHeader.vertex2Size / sizeof(HLSLShared::VertexData2),
                                       sizeof(HLSLShared::VertexData2),
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
                                   size_t& lastTextureOffset)
    {
        UINT skyBoxResolution = sceneHeader.skyBoxResolution;
        UINT skyBoxMipCount = sceneHeader.skyBoxMipCount;

        size_t alignment;
        size_t size;
        Texture2D::GetRequiredSize(alignment, size, device, skyBoxResolution, skyBoxResolution,
                                   skyBoxMipCount, ms_SkyBoxTextureFormat, D3D12_RESOURCE_FLAG_NONE,
                                   BLK_TEXCUBE_FACE_COUNT);

        lastTextureOffset += size;
    }

    void Scene::PrecalculateTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                                     const SceneDataReader::HeaderWrapper& headerWrapper,
                                     size_t& lastTextureOffset, std::vector<size_t>& textureOffsets)
    {
        m_SceneTextures.resize(sceneHeader.textureCount);
        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            const auto& textureHeader = headerWrapper.textureHeaders[i];
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
        }
    }

    void Scene::InitializeSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                                 DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset)
    {
        BLK_CPU_SCOPE("Scene::InitializeSkyBox");

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
                                   const SceneDataReader::HeaderWrapper& headerWrapper,
                                   const std::vector<size_t>& textureOffsets,
                                   DescriptorHeap& mainSRVHeap, UINT mainSRVHeapOffset)
    {
        BLK_CPU_SCOPE("Scene::InitializeTextures");

        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            auto& texture = m_SceneTextures[i];
            const auto& textureHeader = headerWrapper.textureHeaders[i];

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

    void Scene::UploadSkyBox(Device& device, const SceneData::SceneHeader& sceneHeader,
                             UINT64& sourceOffset)
    {
        BLK_CPU_SCOPE("Scene::UploadSkyBox");

        DStorageQueue& dstorageQueue = device.GetDStorageQueue();
        DStorageFile& sourceFile = m_DataReader.GetSceneDataFile();

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

                dstorageQueue.EnququeRead(sourceFile, sourceOffset, textureSize, m_SkyBoxCubemap,
                                          face * skyBoxMipCount + mipNumber, resolution,
                                          resolution);

                resolution >>= 1;
                sourceOffset += textureSize;
            }
        }
    }

    void Scene::UploadTextures(Device& device, const SceneData::SceneHeader& sceneHeader,
                               const SceneDataReader::HeaderWrapper& headerWrapper,
                               UINT64& sourceOffset)
    {
        BLK_CPU_SCOPE("Scene::UploadTextures");

        DStorageQueue& dstorageQueue = device.GetDStorageQueue();
        DStorageFile& sourceFile = m_DataReader.GetSceneDataFile();

        UINT bytesPerPixel = Texture2D::GetBPP(ms_SceneTexturesFormat) / 8;
        BLK_ASSERT(bytesPerPixel != 0);

        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            auto& texture = m_SceneTextures[i];
            auto& textureHeader = headerWrapper.textureHeaders[i];

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

                dstorageQueue.EnququeRead(sourceFile, sourceOffset, textureSize, texture, mipNumber,
                                          width, height);

                width >>= 1;
                height >>= 1;
                sourceOffset += textureSize;
            }
        }
    }

    void Scene::UploadBuffers(Device& device, const SceneData::SceneHeader& sceneHeader,
                              UINT64& sourceOffset)
    {
        BLK_CPU_SCOPE("Scene::UploadBuffers");

        DStorageQueue& dstorageQueue = device.GetDStorageQueue();
        DStorageFile& sourceFile = m_DataReader.GetSceneDataFile();

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.vertex1Size,
                                  m_VertexBuffer1, 0);
        sourceOffset += sceneHeader.vertex1Size;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.vertex2Size,
                                  m_VertexBuffer2, 0);
        sourceOffset += sceneHeader.vertex2Size;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.vertexIndirectionSize,
                                  m_VertexIndirectionBuffer, 0);
        sourceOffset += sceneHeader.vertexIndirectionSize;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.indexSize, m_IndexBuffer,
                                  0);
        sourceOffset += sceneHeader.indexSize;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.meshletsSize,
                                  m_MeshletBuffer, 0);
        sourceOffset += sceneHeader.meshletsSize;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.meshletsCullSize,
                                  m_MeshletCullBuffer, 0);
        sourceOffset += sceneHeader.meshletsCullSize;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.objectsSize, m_ObjectBuffer,
                                  0);
        sourceOffset += sceneHeader.objectsSize;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.materialsSize,
                                  m_MaterialsBuffer, 0);
        sourceOffset += sceneHeader.materialsSize;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.rtIndiciesSize,
                                  m_RTIndexBuffer, 0);
        sourceOffset += sceneHeader.rtIndiciesSize;

        dstorageQueue.EnququeRead(sourceFile, sourceOffset, sceneHeader.rtObjectIndexOffsetSize,
                                  m_RTObjectIndexOffsetBuffer, 0);
        sourceOffset += sceneHeader.rtObjectIndexOffsetSize;
    }

} // namespace Boolka
