#include "stdafx.h"
#include "IndexBuffer.h"

namespace Boolka
{

    bool IndexBuffer::Initialize(Device& device, UINT64 size, void* initialData)
    {
        if (!InitializeCommitedResource(device, size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ))
            return false;

        D3D12_RANGE readRange = {}; // empty read range = no reading

        void* mappedData = nullptr;
        HRESULT hr = m_Resource->Map(0, &readRange, &mappedData);

        if (FAILED(hr))
            return false;

        memcpy(mappedData, initialData, size);

        m_Resource->Unmap(0, nullptr);

        return true;
    }

    void IndexBuffer::Unload()
    {
        BLK_ASSERT(m_Resource != nullptr);
        m_Resource->Release();
        m_Resource = nullptr;
    }

}