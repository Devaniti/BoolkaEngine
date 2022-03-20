#pragma once

namespace Boolka
{

    class Device;

    class [[nodiscard]] ResourceHeap
    {
    public:
        ResourceHeap();
        ~ResourceHeap();

        [[nodiscard]] ID3D12Heap* Get();
        [[nodiscard]] ID3D12Heap* operator->();

        bool Initialize(Device& device, size_t size, D3D12_HEAP_TYPE heapType,
                        D3D12_HEAP_FLAGS heapFlags);
        void Unload();

    private:
        ID3D12Heap* m_Heap;
    };

} // namespace Boolka
