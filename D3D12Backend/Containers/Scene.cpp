#include "stdafx.h"
#include "Scene.h"

#include "Containers/Streaming/SceneData.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "Contexts/RenderEngineContext.h"
#include "APIWrappers/Resources/ResourceTransition.h"


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
        res = m_VertexBuffer.Initialize(device, dataWrapper.vertexBufferSize, dataWrapper.vertexData);
        BLK_ASSERT(res);

        m_IndexBufferSize = dataWrapper.indexBufferSize;
        BLK_ASSERT(m_IndexBufferSize != 0);
        res = m_IndexBuffer.Initialize(device, dataWrapper.indexBufferSize, dataWrapper.indexData);
        BLK_ASSERT(res);

        res = m_SRVDescriptorHeap.Initialize(device, dataWrapper.textureCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        BLK_ASSERT(res);

        size_t totalTextureSize = 0;

        m_Textures.resize(dataWrapper.textureCount);
        m_SRVs.resize(dataWrapper.textureCount);

        static const size_t bytesPerPixel = 4;

        sceneData.PrepareTextureHeaders();

        for (int i = 0; i < dataWrapper.textureCount; ++i)
        {
            auto& texture = m_Textures[i];
            const auto& textureHeader = dataWrapper.textureHeaders[i];

            if (textureHeader.width == 0)
            {
                unsigned char dummy[4] = {};
                texture.Initialize(device, D3D12_HEAP_TYPE_DEFAULT, 1, 1, 1, DXGI_FORMAT_B8G8R8A8_UNORM, D3D12_RESOURCE_FLAG_NONE, nullptr, D3D12_RESOURCE_STATE_COMMON);
            }
            else
            {
                UINT width = textureHeader.width;
                UINT height = textureHeader.height;
                UINT16 mipCount = 0;
                for (; width > 0 && height > 0; ++mipCount)
                {
                    size_t rowPitch = CEIL_TO_POWER_OF_TWO(width * bytesPerPixel, 256);
                    size_t textureSize = CEIL_TO_POWER_OF_TWO(rowPitch * height, 512);
                    totalTextureSize += textureSize;
                    width >>= 1;
                    height >>= 1;
                }

                texture.Initialize(device, D3D12_HEAP_TYPE_DEFAULT, textureHeader.width, textureHeader.height, mipCount, DXGI_FORMAT_B8G8R8A8_UNORM, D3D12_RESOURCE_FLAG_NONE, nullptr, D3D12_RESOURCE_STATE_COMMON);
            }
        }

        UploadBuffer uploadBuffer;
        uploadBuffer.Initialize(device, max(64, totalTextureSize));

        sceneData.PrepareTextures();

        void* mappedBuffer = uploadBuffer.Map();
        memcpy(mappedBuffer, dataWrapper.baseTextureData, totalTextureSize);
        uploadBuffer.Unmap();

        auto& initCommandList = engineContext.GetInitializationCommandList();
        UINT64 textureDataOffset = 0;
        for (int i = 0; i < dataWrapper.textureCount; ++i)
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
                copySource.PlacedFootprint.Offset = textureDataOffset;
                copySource.PlacedFootprint.Footprint.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                copySource.PlacedFootprint.Footprint.Width = width;
                copySource.PlacedFootprint.Footprint.Height = height;
                copySource.PlacedFootprint.Footprint.Depth = 1;
                copySource.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(rowPitch);

                initCommandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySource, nullptr);

                width >>= 1;
                height >>= 1;
                textureDataOffset += textureSize;
            }
        }

        engineContext.FinishInitializationCommandList(device);

        for (int i = 0; i < dataWrapper.textureCount; ++i)
        {
            bool res = m_SRVs[i].Initialize(device, m_Textures[i], m_SRVDescriptorHeap.GetCPUHandle(i));
            BLK_ASSERT(res);
        }

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

        for (auto& srv : m_SRVs)
        {
            srv.Unload();
        }
        m_SRVs.clear();

        m_SRVDescriptorHeap.Unload();

        m_VertexBuffer.Unload();
        m_IndexBuffer.Unload();

        m_VertexBufferSize = 0;
        m_IndexBufferSize = 0;
    }

}
