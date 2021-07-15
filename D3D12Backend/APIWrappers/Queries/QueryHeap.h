#pragma once

namespace Boolka
{

    class QueryHeap
    {
    public:
        QueryHeap();
        ~QueryHeap();

        ID3D12QueryHeap* Get();
        ID3D12QueryHeap* operator->();

        bool Initialize(Device& device, D3D12_QUERY_HEAP_TYPE heapType, UINT elementCount);
        void Unload();

    private:
        ID3D12QueryHeap* m_heap;
    };

} // namespace Boolka
