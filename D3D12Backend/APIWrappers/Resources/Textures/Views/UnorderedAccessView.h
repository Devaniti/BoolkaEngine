#pragma once

namespace Boolka
{
    class Device;
    class Texture2D;
    class Buffer;

    class UnorderedAccessView
    {
    public:
        static void Initialize(Device& device, Texture2D& texture, DXGI_FORMAT format,
                               D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
    };

} // namespace Boolka
