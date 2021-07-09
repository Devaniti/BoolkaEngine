#include "stdafx.h"

#include "QueryHeap.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    QueryHeap::QueryHeap()
        : m_heap(nullptr)
    {
    }

    QueryHeap::~QueryHeap()
    {
        BLK_ASSERT(m_heap == nullptr);
    }

    ID3D12QueryHeap* QueryHeap::Get()
    {
        BLK_ASSERT(m_heap != nullptr);
        return m_heap;
    }

    ID3D12QueryHeap* QueryHeap::operator->()
    {
        return Get();
    }

    bool QueryHeap::Initialize(Device& device, D3D12_QUERY_HEAP_TYPE heapType, UINT elementCount)
    {
        BLK_ASSERT(m_heap == nullptr);

        D3D12_QUERY_HEAP_DESC desc{};
        desc.Type = heapType;
        desc.Count = elementCount;
        HRESULT hr = device->CreateQueryHeap(&desc, IID_PPV_ARGS(&m_heap));
        BLK_ASSERT(SUCCEEDED(hr));
        return SUCCEEDED(hr);
    }

    void QueryHeap::Unload()
    {
        BLK_ASSERT(m_heap != nullptr)

        m_heap->Release();
        m_heap = nullptr;
    }

} // namespace Boolka
