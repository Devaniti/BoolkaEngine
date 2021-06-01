#include "stdafx.h"

#include "ReadbackBuffer.h"

namespace Boolka
{

    bool ReadbackBuffer::Initialize(Device& device, UINT64 size)
    {
        if (!Buffer::Initialize(device, size, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_FLAG_NONE,
                                D3D12_RESOURCE_STATE_COPY_DEST))
            return false;

        return true;
    }

    void ReadbackBuffer::Unload()
    {
        BLK_ASSERT(m_Resource != nullptr);
        m_Resource->Release();
        m_Resource = nullptr;
    }

    void* ReadbackBuffer::Map(UINT64 readRangeBegin, UINT64 readRangeEnd)
    {
        D3D12_RANGE readRange = {readRangeBegin, readRangeEnd};
        void* result = nullptr;
        HRESULT hr = m_Resource->Map(0, &readRange, &result);
        BLK_ASSERT_VAR(SUCCEEDED(hr));
        return result;
    }

    void ReadbackBuffer::Unmap()
    {
        D3D12_RANGE writeRange = {}; // empty read range = no writing
        m_Resource->Unmap(0, &writeRange);
    }

    void ReadbackBuffer::Readback(void* data, UINT64 size)
    {
        void* result = Map(0, size);
        memcpy(data, result, size);
        Unmap();
    }

} // namespace Boolka