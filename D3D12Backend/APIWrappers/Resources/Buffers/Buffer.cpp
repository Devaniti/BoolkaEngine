#include "stdafx.h"

#include "Buffer.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    bool Buffer::Initialize(Device& device, UINT64 size, D3D12_HEAP_TYPE heapType,
                            D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialState)
    {
        BLK_ASSERT(m_Resource == nullptr);
        BLK_ASSERT(initialState == D3D12_RESOURCE_STATE_COMMON ||
                   initialState == D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = heapType;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = size;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = resourceFlags;

        HRESULT hr =
            device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                            initialState, nullptr, IID_PPV_ARGS(&m_Resource));

        if (FAILED(hr))
            return false;

        return true;
    }

    void Buffer::Unload()
    {
        BLK_ASSERT(m_Resource != nullptr);
        m_Resource->Release();
        m_Resource = nullptr;
    }

} // namespace Boolka