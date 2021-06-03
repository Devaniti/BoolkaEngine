#include "stdafx.h"

#include "ConstantBufferView.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    bool ConstantBufferView::Initialize(Device& device, Buffer& constantBuffer,
                                        D3D12_CPU_DESCRIPTOR_HANDLE destinationDescriptorHandle,
                                        UINT size)
    {
        D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
        desc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
        desc.SizeInBytes = size;
        device->CreateConstantBufferView(&desc, destinationDescriptorHandle);

        return true;
    }

} // namespace Boolka