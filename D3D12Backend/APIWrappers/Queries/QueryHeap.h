#pragma once

namespace Boolka
{

    class [[nodiscard]] QueryHeap
    {
    public:
        QueryHeap();
        ~QueryHeap();

        [[nodiscard]] ID3D12QueryHeap* Get();
        [[nodiscard]] ID3D12QueryHeap* operator->();

        bool Initialize(Device& device, D3D12_QUERY_HEAP_TYPE heapType, UINT elementCount);
        void Unload();

    private:
        ID3D12QueryHeap* m_heap;
    };

} // namespace Boolka
