#pragma once

namespace Boolka
{
    class Device;
    class Texture2D;
    class Buffer;

    class ShaderResourceView
    {
    public:
        ShaderResourceView();
        ~ShaderResourceView();

        bool Initialize(Device& device, Texture2D& texture,
                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        bool Initialize(Device& device, Texture2D& texture,
                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, DXGI_FORMAT format);
        bool Initialize(Device& device, Buffer& buffer, UINT elementCount, UINT stride,
                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        bool InitializeCube(Device& device, Texture2D& texture,
                            D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, DXGI_FORMAT format);
        void Unload();

        static void CreateSRV(Device& device, Texture2D& texture,
                              D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void CreateSRV(Device& device, Texture2D& texture,
                              D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, DXGI_FORMAT format);
        static void CreateSRV(Device& device, Buffer& buffer, UINT elementCount, UINT stride,
                              D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void CreateSRVCube(Device& device, Texture2D& texture,
                                  D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, DXGI_FORMAT format);

        D3D12_CPU_DESCRIPTOR_HANDLE* GetCPUDescriptor();

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescriptorHandle;
    };

} // namespace Boolka
