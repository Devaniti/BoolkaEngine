#pragma once

namespace Boolka
{
    class Device;
    class Texture2D;
    class Buffer;

    class UnorderedAccessView
    {
    public:
        static void Initialize(Device& device, Texture1D& texture, DXGI_FORMAT format,
                               D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void Initialize(Device& device, Texture2D& texture, DXGI_FORMAT format,
                               D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void Initialize(Device& device, Buffer& buffer, UINT stride, UINT elementCount,
                               D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void Initialize(Device& device, Buffer& buffer, DXGI_FORMAT format,
                               UINT elementCount, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void InitializeWithCounter(Device& device, Buffer& buffer, UINT stride,
                                          UINT elementCount,
                                          D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void InitializeWithCounter(Device& device, Buffer& buffer, DXGI_FORMAT format,
                                          UINT elementCount,
                                          D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);

    private:
        static void Initialize(Device& device, Buffer& buffer, DXGI_FORMAT format, UINT stride,
                               UINT elementCount, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        static void InitializeWithCounter(Device& device, Buffer& buffer, DXGI_FORMAT format,
                                          UINT stride, UINT elementCount,
                                          D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
    };

} // namespace Boolka
