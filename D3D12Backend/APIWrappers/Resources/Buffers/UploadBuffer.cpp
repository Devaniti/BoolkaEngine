#include "stdafx.h"

#include "UploadBuffer.h"

namespace Boolka
{

    bool UploadBuffer::Initialize(Device& device, UINT64 size)
    {
        if (!Buffer::Initialize(device, size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_FLAG_NONE))
            return false;

        return true;
    }

    void UploadBuffer::Unload()
    {
        BLK_ASSERT(m_Resource != nullptr);
        m_Resource->Release();
        m_Resource = nullptr;
    }

    void* UploadBuffer::Map()
    {
        D3D12_RANGE readRange = {}; // empty read range = no reading
        void* result = nullptr;
        HRESULT hr = m_Resource->Map(0, &readRange, &result);
        BLK_ASSERT_VAR(SUCCEEDED(hr));
        return result;
    }

    void UploadBuffer::Unmap()
    {
        m_Resource->Unmap(0, nullptr);
    }

    void UploadBuffer::Upload(const void* data, UINT64 size)
    {
        void* dst = Map();
        memcpy(dst, data, size);
        Unmap();
    }

} // namespace Boolka