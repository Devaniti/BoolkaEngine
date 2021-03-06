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
