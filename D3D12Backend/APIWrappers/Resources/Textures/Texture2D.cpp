#include "stdafx.h"

#include "Texture2D.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/ResourceHeap.h"

namespace Boolka
{

    bool Texture2D::Initialize(ID3D12Resource* resource)
    {
        BLK_ASSERT(m_Resource == nullptr);
#ifdef BLK_DEBUG
        D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();
        BLK_ASSERT(resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D);
#endif

        m_Resource = resource;

        return true;
    }

    bool Texture2D::Initialize(Device& device, D3D12_HEAP_TYPE heapType, UINT64 width, UINT height,
                               UINT16 mipCount, DXGI_FORMAT format,
                               D3D12_RESOURCE_FLAGS resourceFlags, D3D12_CLEAR_VALUE* clearValue,
                               D3D12_RESOURCE_STATES initialState, UINT16 arraySize /*= 1*/)
    {
        BLK_ASSERT(m_Resource == nullptr);

        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = heapType;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC resourceDesc =
            FillDesc(width, height, mipCount, format, resourceFlags, arraySize);

        HRESULT hr =
            device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                            initialState, clearValue, IID_PPV_ARGS(&m_Resource));
        BLK_ASSERT(SUCCEEDED(hr));
        return SUCCEEDED(hr);
    }

    bool Texture2D::Initialize(Device& device, ResourceHeap& resourceHeap, size_t heapOffset,
                               UINT64 width, UINT height, UINT16 mipCount, DXGI_FORMAT format,
                               D3D12_RESOURCE_FLAGS resourceFlags, D3D12_CLEAR_VALUE* clearValue,
                               D3D12_RESOURCE_STATES initialState, UINT16 arraySize /*= 1*/)
    {
        D3D12_RESOURCE_DESC resourceDesc =
            FillDesc(width, height, mipCount, format, resourceFlags, arraySize);

        HRESULT hr =
            device->CreatePlacedResource(resourceHeap.Get(), heapOffset, &resourceDesc,
                                         initialState, clearValue, IID_PPV_ARGS(&m_Resource));
        BLK_ASSERT(SUCCEEDED(hr));
        return SUCCEEDED(hr);
    }

    void Texture2D::GetRequiredSize(size_t& outAlignment, size_t& outSize, Device& device,
                                    UINT64 width, UINT height, UINT16 mipCount, DXGI_FORMAT format,
                                    D3D12_RESOURCE_FLAGS resourceFlags, UINT16 arraySize /*= 1*/)
    {
        D3D12_RESOURCE_DESC resourceDesc =
            FillDesc(width, height, mipCount, format, resourceFlags, arraySize);

        D3D12_RESOURCE_ALLOCATION_INFO allocationInfo =
            device->GetResourceAllocationInfo(0, 1, &resourceDesc);
        outAlignment = allocationInfo.Alignment;
        outSize = allocationInfo.SizeInBytes;
    }

    size_t Texture2D::GetUploadSize(UINT width, UINT height, UINT16 mipCount, DXGI_FORMAT format,
                                    D3D12_RESOURCE_FLAGS resourceFlags, UINT16 arraySize /*= 1*/)
    {
        size_t result = 0;

        UINT bytesPerPixel = GetBPP(format) / 8;
        BLK_ASSERT(bytesPerPixel > 0);

        UINT currentWidth = width;
        UINT currentHeight = height;
        for (UINT16 currentMip = 0; currentMip < mipCount; ++currentMip)
        {
            BLK_ASSERT(currentWidth != 0);
            BLK_ASSERT(currentHeight != 0);

            size_t rowPitch = BLK_CEIL_TO_POWER_OF_TWO(currentWidth * bytesPerPixel,
                                                       D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
            size_t textureSize = BLK_CEIL_TO_POWER_OF_TWO(rowPitch * currentHeight,
                                                          D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
            result += textureSize;
            currentWidth >>= 1;
            currentHeight >>= 1;
        }

        return result * arraySize;
    }

    UINT Texture2D::GetBPP(DXGI_FORMAT format)
    {
        switch (format)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return 128;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            return 96;

        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_Y416:
        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
            return 64;

        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_AYUV:
        case DXGI_FORMAT_Y410:
        case DXGI_FORMAT_YUY2:
            return 32;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
            return 24;

        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_A8P8:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            return 16;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE:
        case DXGI_FORMAT_NV11:
            return 12;

        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_AI44:
        case DXGI_FORMAT_IA44:
        case DXGI_FORMAT_P8:
            return 8;

        case DXGI_FORMAT_R1_UNORM:
            return 1;

        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            return 4;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return 8;

        default:
            BLK_ASSERT(0);
            return 0;
        }
    }

    void Texture2D::Unload()
    {
        BLK_ASSERT(m_Resource);

        m_Resource->Release();
        m_Resource = nullptr;
    }

    D3D12_RESOURCE_DESC Texture2D::FillDesc(UINT64 width, UINT height, UINT16 mipCount,
                                            DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags,
                                            UINT16 arraySize)
    {
        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = width;
        resourceDesc.Height = height;
        resourceDesc.DepthOrArraySize = arraySize;
        resourceDesc.MipLevels = mipCount;
        resourceDesc.Format = format;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = resourceFlags;

        return resourceDesc;
    }

} // namespace Boolka
