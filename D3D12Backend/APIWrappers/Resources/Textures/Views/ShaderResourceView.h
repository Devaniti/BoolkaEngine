#pragma once

namespace Boolka
{
    class Device;
    class Texture2D;
    class Buffer;

    class ShaderResourceView
    {
    public:
        static void Initialize(Device& device, Texture2D& texture,
                               D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void Initialize(Device& device, Texture2D& texture,
                               D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, DXGI_FORMAT format);
        static void Initialize(Device& device, Buffer& buffer, UINT elementCount, UINT stride,
                               D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void InitializeCube(Device& device, Texture2D& texture,
                                   D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor, DXGI_FORMAT format);
        static void InitializeAccelerationStructure(Device& device,
                                                    D3D12_GPU_VIRTUAL_ADDRESS tlasAddress,
                                                    D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void InitializeNullDescriptorTexture2D(Device& device, DXGI_FORMAT format,
                                                      D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
    };

} // namespace Boolka
