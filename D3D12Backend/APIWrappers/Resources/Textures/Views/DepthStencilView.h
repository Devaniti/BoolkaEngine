#pragma once

namespace Boolka
{
    class Device;
    class Texture2D;

    class DepthStencilView
    {
    public:
        DepthStencilView();
        ~DepthStencilView();

        bool Initialize(Device& device, Texture2D& texture, DXGI_FORMAT format, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, UINT16 arraySlice = 0);
        void Unload();

        D3D12_CPU_DESCRIPTOR_HANDLE* GetCPUDescriptor() { BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0); return &m_CPUDescriptorHandle; };

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescriptorHandle;
    };

}
