#include "stdafx.h"
#include "UploadBuffer.h"

namespace Boolka
{

    UploadBuffer::UploadBuffer()
    {
    }

    UploadBuffer::~UploadBuffer()
    {
    }

    bool UploadBuffer::Initialize(Device& device, UINT64 size)
    {
        if (!InitializeCommitedResource(device, size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ))
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
        m_Resource->Map(0, &readRange, &result);
        return result;
    }

    void UploadBuffer::Unmap()
    {
        m_Resource->Unmap(0, nullptr);
    }

}