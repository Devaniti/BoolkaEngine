#pragma once

namespace Boolka
{

    class Device;

    class ResourceHeap
    {
    public:
        ResourceHeap();
        ~ResourceHeap();

        ID3D12Heap* Get() { BLK_ASSERT(m_heap != nullptr); return m_heap; };
        ID3D12Heap* operator->() { return Get(); };

        bool Initialize(Device& device, size_t size, D3D12_HEAP_TYPE heapType, D3D12_HEAP_FLAGS heapFlags);
        void Unload();

    private:
        ID3D12Heap* m_heap;
    };

}

