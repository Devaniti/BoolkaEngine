#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class ConstantBufferView
    {
    public:
        static bool Initialize(Device& device, Buffer& constantBuffer,
                               D3D12_CPU_DESCRIPTOR_HANDLE destinationDescriptorHandle, UINT size);
    };

} // namespace Boolka
