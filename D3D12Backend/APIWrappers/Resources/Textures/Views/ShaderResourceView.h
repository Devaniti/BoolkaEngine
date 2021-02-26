#pragma once

namespace Boolka
{
    class Device;
    class Texture2D;

    class ShaderResourceView
    {
    public:
        ShaderResourceView();
        ~ShaderResourceView();

        bool Initialize(Device& device, Texture2D& texture, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        bool Initialize(Device& device, Texture2D& texture, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, DXGI_FORMAT format);
        bool InitializeCube(Device& device, Texture2D& texture, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, DXGI_FORMAT format);
        void Unload();

        D3D12_CPU_DESCRIPTOR_HANDLE* GetCPUDescriptor() { BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0); return &m_CPUDescriptorHandle; };

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescriptorHandle;
    };

}
