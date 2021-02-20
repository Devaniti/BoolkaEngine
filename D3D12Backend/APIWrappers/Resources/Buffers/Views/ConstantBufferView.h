#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class ConstantBufferView
    {
    public:
        ConstantBufferView();
        ~ConstantBufferView();

        bool Initialize(Device& device, Buffer& constantBuffer, D3D12_CPU_DESCRIPTOR_HANDLE destinationDescriptorHandle, UINT size);
        void Unload();

        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptor() { BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0); return m_CPUDescriptorHandle; };

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescriptorHandle;
    };

}
