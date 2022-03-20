#include "stdafx.h"

#include "ResourceHeap.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    ResourceHeap::ResourceHeap()
        : m_Heap(nullptr)
    {
    }

    ResourceHeap::~ResourceHeap()
    {
        BLK_ASSERT(m_Heap == nullptr);
    }

    ID3D12Heap* ResourceHeap::Get()
    {
        BLK_ASSERT(m_Heap != nullptr);
        return m_Heap;
    }

    ID3D12Heap* ResourceHeap::operator->()
    {
        return Get();
    }

    bool ResourceHeap::Initialize(Device& device, size_t size, D3D12_HEAP_TYPE heapType,
                                  D3D12_HEAP_FLAGS heapFlags)
    {
        BLK_ASSERT(m_Heap == nullptr);

        BLK_CPU_SCOPE("ResourceHeap::Initialize");

        D3D12_HEAP_DESC desc = {};
        desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        desc.Flags = heapFlags;
        desc.SizeInBytes = size;
        desc.Properties.Type = heapType;
        desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        HRESULT hr = device->CreateHeap(&desc, IID_PPV_ARGS(&m_Heap));
        BLK_ASSERT(SUCCEEDED(hr));

        return SUCCEEDED(hr);
    }

    void ResourceHeap::Unload()
    {
        BLK_ASSERT(m_Heap != nullptr);

        m_Heap->Release();
        m_Heap = nullptr;
    }

} // namespace Boolka
