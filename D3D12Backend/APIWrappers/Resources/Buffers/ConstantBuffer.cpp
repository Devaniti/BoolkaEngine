#include "stdafx.h"
#include "ConstantBuffer.h"

namespace Boolka
{

    bool ConstantBuffer::Initialize(Device& device, UINT64 size)
    {
        if (!InitializeCommitedResource(device, size, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST))
            return false;

        return true;
    }

    void ConstantBuffer::Unload()
    {
        BLK_ASSERT(m_Resource != nullptr);
        m_Resource->Release();
        m_Resource = nullptr;
    }

}