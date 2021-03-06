#pragma once

namespace Boolka
{

    class Device;

    class ResourceHeap
    {
    public:
        ResourceHeap();
        ~ResourceHeap();

        ID3D12Heap* Get();
        ID3D12Heap* operator->();

        bool Initialize(Device& device, size_t size, D3D12_HEAP_TYPE heapType,
                        D3D12_HEAP_FLAGS heapFlags);
        void Unload();

    private:
        ID3D12Heap* m_heap;
    };

} // namespace Boolka
