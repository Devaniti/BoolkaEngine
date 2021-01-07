#include "stdafx.h"
#include "Scene.h"

#include "Containers/Streaming/SceneData.h"
#include "Contexts/RenderEngineContext.h"
#include "APIWrappers/Resources/ResourceTransition.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"


namespace Boolka
{

    Scene::Scene()
        : m_VertexBufferSize(0)
        , m_IndexBufferSize(0)
    {
    }

    Scene::~Scene()
    {
        BLK_ASSERT(m_VertexBufferSize == 0);
        BLK_ASSERT(m_IndexBufferSize == 0);
    }

    bool Scene::Initialize(Device& device, SceneData& sceneData, RenderEngineContext& engineContext)
    {
        BLK_ASSERT(m_VertexBufferSize == 0);
        BLK_ASSERT(m_IndexBufferSize == 0);

        const auto dataWrapper = sceneData.GetSceneWrapper();
        bool res;

        m_VertexBufferSize = dataWrapper.vertexBufferSize;
        BLK_ASSERT(m_VertexBufferSize != 0);
        res = m_VertexBuffer.Initialize(device, m_VertexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT(res);

        res = m_VertexBufferView.Initialize(m_VertexBuffer, m_VertexBufferSize, 9 * sizeof(float));
        BLK_ASSERT(res);

        m_IndexBufferSize = dataWrapper.indexBufferSize;
        BLK_ASSERT(m_IndexBufferSize != 0);
        res = m_IndexBuffer.Initialize(device, m_IndexBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT(res);

        res = m_IndexBufferView.Initialize(m_IndexBuffer, m_IndexBufferSize, DXGI_FORMAT_R32_UINT);
        BLK_ASSERT(res);

        m_IndexCount = dataWrapper.indexCount;
        BLK_ASSERT(m_IndexCount);

        m_CullingBufferSize = dataWrapper.cullingBufferSize;
        BLK_ASSERT(m_CullingBufferSize != 0);
        res = m_CullingBuffer.Initialize(device, m_CullingBufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        BLK_ASSERT(res);

        m_ObjectCount = dataWrapper.objectCount;
        BLK_ASSERT(m_ObjectCount != 0);

        res = m_CommandSignature.Initialize(device);
        BLK_ASSERT(res);

        res = m_DrawIndirectBuffer.Initialize(device, (static_cast<UINT64>(m_ObjectCount) + 1) * 32, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        BLK_ASSERT(res);

        res = m_SRVDescriptorHeap.Initialize(device, dataWrapper.textureCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        BLK_ASSERT(res);

        m_Textures.resize(dataWrapper.textureCount);
        m_SRVs.resize(dataWrapper.textureCount);

        static const size_t bytesPerPixel = 4;

        sceneData.PrepareTextureHeaders();

        size_t uploadSize = static_cast<size_t>(m_VertexBufferSize) + m_IndexBufferSize + m_CullingBufferSize;
        std::vector<size_t> textureOffsets;
        textureOffsets.reserve(dataWrapper.textureCount);

        size_t lastOffset = 0;
        for (UINT i = 0; i < dataWrapper.textureCount; ++i)
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
                size_t rowPitch = CEIL_TO_POWER_OF_TWO(currentWidth * bytesPerPixel, 256);
                size_t textureSize = CEIL_TO_POWER_OF_TWO(rowPitch * currentHeight, 512);
                uploadSize += textureSize;
                currentWidth >>= 1;
                currentHeight >>= 1;
            }
            BLK_ASSERT(mipCount == currentMip);
            size_t alignment;
            size_t size;
            Texture2D::GetRequiredSize(device, textureHeader.width, textureHeader.height, mipCount, DXGI_FORMAT_B8G8R8A8_UNORM, D3D12_RESOURCE_FLAG_NONE, alignment, size);

            lastOffset = CEIL_TO_POWER_OF_TWO(lastOffset, alignment);
            textureOffsets.push_back(lastOffset);
            lastOffset += size;
        }
        size_t heapSize = lastOffset;

        m_ResourceHeap.Initialize(device, heapSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES);

        for (UINT i = 0; i < dataWrapper.textureCount; ++i)
        {
            auto& texture = m_Textures[i];
            const auto& textureHeader = dataWrapper.textureHeaders[i];

            texture.Initialize(device, m_ResourceHeap, textureOffsets[i], textureHeader.width, textureHeader.height, textureHeader.mipCount, DXGI_FORMAT_B8G8R8A8_UNORM, D3D12_RESOURCE_FLAG_NONE, nullptr, D3D12_RESOURCE_STATE_COMMON);
        }

        for (UINT i = 0; i < dataWrapper.textureCount; ++i)
        {
            bool res = m_SRVs[i].Initialize(device, m_Textures[i], m_SRVDescriptorHeap.GetCPUHandle(i));
            BLK_ASSERT(res);
        }

        UploadBuffer uploadBuffer;
        uploadBuffer.Initialize(device, max(64, uploadSize));


        DebugProfileTimer streamingWait;
        streamingWait.Start();
        sceneData.PrepareBinaryData();
        streamingWait.Stop(L"Streaming wait");

        uploadBuffer.Upload(dataWrapper.binaryData, uploadSize);

        auto& initCommandList = engineContext.GetInitializationCommandList();

        UINT64 uploadBufferOffset = 0;

        initCommandList->CopyBufferRegion(m_VertexBuffer.Get(), 0, uploadBuffer.Get(), uploadBufferOffset, m_VertexBufferSize);
        uploadBufferOffset += m_VertexBufferSize;

        initCommandList->CopyBufferRegion(m_IndexBuffer.Get(), 0, uploadBuffer.Get(), uploadBufferOffset, m_IndexBufferSize);
        uploadBufferOffset += m_IndexBufferSize;

        initCommandList->CopyBufferRegion(m_CullingBuffer.Get(), 0, uploadBuffer.Get(), uploadBufferOffset, m_CullingBufferSize);
        uploadBufferOffset += m_CullingBufferSize;

        for (UINT i = 0; i < dataWrapper.textureCount; ++i)
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
                size_t rowPitch = CEIL_TO_POWER_OF_TWO(width * bytesPerPixel, 256);
                size_t textureSize = CEIL_TO_POWER_OF_TWO(rowPitch * height, 512);
            
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

        return true;
    }

    void Scene::Unload()
    {
        BLK_ASSERT(m_VertexBufferSize != 0);
        BLK_ASSERT(m_IndexBufferSize != 0);

        for (auto& texture : m_Textures)
        {
            texture.Unload();
        }
        m_Textures.clear();

        m_ResourceHeap.Unload();

        for (auto& srv : m_SRVs)
        {
            srv.Unload();
        }
        m_SRVs.clear();

        m_SRVDescriptorHeap.Unload();

        m_VertexBufferView.Unload();
        m_IndexBufferView.Unload();

        m_VertexBuffer.Unload();
        m_IndexBuffer.Unload();
        m_CullingBuffer.Unload();
        m_DrawIndirectBuffer.Unload();

        m_CommandSignature.Unload();

        m_VertexBufferSize = 0;
        m_IndexBufferSize = 0;
    }

}
