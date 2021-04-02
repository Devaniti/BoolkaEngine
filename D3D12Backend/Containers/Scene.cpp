#include "stdafx.h"

#include "Scene.h"

#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/ResourceTransition.h"
#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"
#include "Contexts/RenderEngineContext.h"

namespace Boolka
{

    const UINT Scene::ms_MeshletSRVCount = 6;

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

        const auto dataWrapper = sceneData.GetSceneWrapper();
        const auto sceneHeader = dataWrapper.header;
        bool res;

        res = m_VertexBuffer1.Initialize(device, sceneHeader.vertex1Size, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);

        res = m_VertexBuffer2.Initialize(device, sceneHeader.vertex2Size, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);

        res = m_VertexIndirectionBuffer.Initialize(
            device, sceneHeader.vertexIndirectionSize, D3D12_HEAP_TYPE_DEFAULT,
            D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);

        res = m_IndexBuffer.Initialize(device, sceneHeader.indexSize, D3D12_HEAP_TYPE_DEFAULT,
                                       D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);

        res = m_MeshletBuffer.Initialize(device, sceneHeader.meshletsSize, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);

        res = m_ObjectBuffer.Initialize(device, sceneHeader.objectsSize, D3D12_HEAP_TYPE_DEFAULT,
                                        D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT_VAR(res);

        res = m_SRVDescriptorHeap.Initialize(device, ms_MeshletSRVCount + sceneHeader.textureCount,
                                             D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                             D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        BLK_ASSERT_VAR(res);

        m_Textures.resize(sceneHeader.textureCount);

        static const size_t bytesPerPixel = 4;

        sceneData.PrepareTextureHeaders();

        size_t uploadSize = static_cast<size_t>(sceneHeader.vertex1Size) + sceneHeader.vertex2Size +
                            sceneHeader.vertexIndirectionSize + sceneHeader.indexSize +
                            sceneHeader.meshletsSize + sceneHeader.objectsSize;
        std::vector<size_t> textureOffsets;
        textureOffsets.reserve(sceneHeader.textureCount);

        size_t lastOffset = 0;
        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            const auto& textureHeader = dataWrapper.textureHeaders[i];
            UINT width = textureHeader.width;
            UINT height = textureHeader.height;
            UINT mipCount = textureHeader.mipCount;
            BLK_ASSERT(width != 0);
            BLK_ASSERT(height != 0);

            UINT currentWidth = width;
            UINT currentHeight = height;
            UINT16 currentMip = 0;
            for (; currentWidth > 0 && currentHeight > 0; ++currentMip)
            {
                size_t rowPitch = BLK_CEIL_TO_POWER_OF_TWO(currentWidth * bytesPerPixel, 256);
                size_t textureSize = BLK_CEIL_TO_POWER_OF_TWO(rowPitch * currentHeight, 512);
                uploadSize += textureSize;
                currentWidth >>= 1;
                currentHeight >>= 1;
            }
            BLK_ASSERT(mipCount == currentMip);
            size_t alignment;
            size_t size;
            Texture2D::GetRequiredSize(alignment, size, device, textureHeader.width,
                                       textureHeader.height, mipCount, DXGI_FORMAT_B8G8R8A8_UNORM,
                                       D3D12_RESOURCE_FLAG_NONE);

            lastOffset = BLK_CEIL_TO_POWER_OF_TWO(lastOffset, alignment);
            textureOffsets.push_back(lastOffset);
            lastOffset += size;
        }
        size_t heapSize = lastOffset;

        m_ResourceHeap.Initialize(device, heapSize, D3D12_HEAP_TYPE_DEFAULT,
                                  D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES);

        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            auto& texture = m_Textures[i];
            const auto& textureHeader = dataWrapper.textureHeaders[i];

            texture.Initialize(device, m_ResourceHeap, textureOffsets[i], textureHeader.width,
                               textureHeader.height, textureHeader.mipCount,
                               DXGI_FORMAT_B8G8R8A8_UNORM, D3D12_RESOURCE_FLAG_NONE, nullptr,
                               D3D12_RESOURCE_STATE_COMMON);
            BLK_RENDER_DEBUG_ONLY(texture.SetDebugName(L"Scene::m_Textures[%d]", i));
        }

        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            ShaderResourceView::CreateSRV(device, m_Textures[i],
                                          m_SRVDescriptorHeap.GetCPUHandle(ms_MeshletSRVCount + i));
            BLK_ASSERT_VAR(res);
        }

        UINT slot = 0;
        ShaderResourceView::CreateSRV(
            device, m_VertexBuffer1, sceneHeader.vertex1Size / sizeof(SceneData::VertexData1),
            sizeof(SceneData::VertexData1), m_SRVDescriptorHeap.GetCPUHandle(slot++));

        ShaderResourceView::CreateSRV(
            device, m_VertexBuffer2, sceneHeader.vertex1Size / sizeof(SceneData::VertexData1),
            sizeof(SceneData::VertexData1), m_SRVDescriptorHeap.GetCPUHandle(slot++));

        ShaderResourceView::CreateSRV(device, m_VertexIndirectionBuffer,
                                      sceneHeader.vertexIndirectionSize / sizeof(uint32_t),
                                      sizeof(uint32_t), m_SRVDescriptorHeap.GetCPUHandle(slot++));

        ShaderResourceView::CreateSRV(device, m_IndexBuffer,
                                      sceneHeader.indexSize / sizeof(uint32_t), sizeof(uint32_t),
                                      m_SRVDescriptorHeap.GetCPUHandle(slot++));

        ShaderResourceView::CreateSRV(
            device, m_MeshletBuffer, sceneHeader.meshletsSize / sizeof(SceneData::MeshletData),
            sizeof(SceneData::MeshletData), m_SRVDescriptorHeap.GetCPUHandle(slot++));

        ShaderResourceView::CreateSRV(
            device, m_ObjectBuffer, sceneHeader.objectsSize / sizeof(SceneData::ObjectHeader),
            sizeof(SceneData::ObjectHeader), m_SRVDescriptorHeap.GetCPUHandle(slot++));

        BLK_ASSERT(slot == ms_MeshletSRVCount);

        UploadBuffer uploadBuffer;
        uploadBuffer.Initialize(device, std::max(size_t(64), uploadSize));

        DebugProfileTimer streamingWait;
        streamingWait.Start();
        sceneData.PrepareBinaryData();
        streamingWait.Stop(L"Streaming wait");

        uploadBuffer.Upload(dataWrapper.binaryData, uploadSize);

        auto& initCommandList = engineContext.GetInitializationCommandList();

        UINT64 uploadBufferOffset = 0;

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

        initCommandList->CopyBufferRegion(m_ObjectBuffer.Get(), 0, uploadBuffer.Get(),
                                          uploadBufferOffset, sceneHeader.objectsSize);
        uploadBufferOffset += sceneHeader.objectsSize;

        for (UINT i = 0; i < sceneHeader.textureCount; ++i)
        {
            auto& texture = m_Textures[i];
            auto& textureHeader = dataWrapper.textureHeaders[i];

            if (textureHeader.width == 0)
                continue;

            UINT width = textureHeader.width;
            UINT height = textureHeader.height;
            UINT16 mipNumber = 0;
            for (; width > 0 && height > 0; ++mipNumber)
            {
                size_t rowPitch = BLK_CEIL_TO_POWER_OF_TWO(width * bytesPerPixel, 256);
                size_t textureSize = BLK_CEIL_TO_POWER_OF_TWO(rowPitch * height, 512);

                D3D12_TEXTURE_COPY_LOCATION copyDest;
                copyDest.pResource = texture.Get();
                copyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                copyDest.SubresourceIndex = mipNumber;

                D3D12_TEXTURE_COPY_LOCATION copySource;
                copySource.pResource = uploadBuffer.Get();
                copySource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                copySource.PlacedFootprint.Offset = uploadBufferOffset;
                copySource.PlacedFootprint.Footprint.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

        engineContext.FinishInitializationCommandList(device);

        uploadBuffer.Unload();

        m_ObjectCount = sceneHeader.objectCount;
        m_OpaqueObjectCount = sceneHeader.opaqueCount;

        m_BatchManager.Initialize(*this);

        return true;
    }

    void Scene::Unload()
    {
        for (auto& texture : m_Textures)
        {
            texture.Unload();
        }
        m_Textures.clear();

        m_ResourceHeap.Unload();

        m_SRVDescriptorHeap.Unload();

        m_VertexBuffer1.Unload();
        m_VertexBuffer2.Unload();
        m_VertexIndirectionBuffer.Unload();
        m_IndexBuffer.Unload();
        m_MeshletBuffer.Unload();
        m_ObjectBuffer.Unload();

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

    D3D12_GPU_DESCRIPTOR_HANDLE Scene::GetSceneTexturesTable() const
    {
        return m_SRVDescriptorHeap.GetGPUHandle(ms_MeshletSRVCount);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE Scene::GetMeshletsTable() const
    {
        return m_SRVDescriptorHeap.GetGPUHandle(0);
    }

    DescriptorHeap& Scene::GetSRVDescriptorHeap()
    {
        return m_SRVDescriptorHeap;
    }

    BatchManager& Scene::GetBatchManager()
    {
        return m_BatchManager;
    }

    void Scene::BindResources(CommandList& commandList)
    {
        ID3D12DescriptorHeap* descriptorHeaps[] = {GetSRVDescriptorHeap().Get()};
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::SceneSRV),
            GetSceneTexturesTable());
        commandList->SetGraphicsRootDescriptorTable(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::SceneMeshletsSRV),
            GetMeshletsTable());
    }

} // namespace Boolka
