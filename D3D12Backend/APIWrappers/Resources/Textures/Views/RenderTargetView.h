#pragma once

namespace Boolka
{
    class Device;
    class Texture2D;

    class RenderTargetView
    {
    public:
        RenderTargetView();
        ~RenderTargetView();

        bool Initialize(Device& device, Texture2D& texture, DXGI_FORMAT format,
                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        void Unload();

        D3D12_CPU_DESCRIPTOR_HANDLE* GetCPUDescriptor();

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescriptorHandle;
    };

} // namespace Boolka
