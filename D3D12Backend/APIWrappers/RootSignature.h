#pragma once

namespace Boolka
{
    class Device;

    class RootSignature
    {
    public:
        RootSignature();
        ~RootSignature();

        ID3D12RootSignature* Get();
        ID3D12RootSignature* operator->();

        bool Initialize(Device& device, const char* filename);
        void Unload();

    private:
        ID3D12RootSignature* m_RootSignature;
    };

} // namespace Boolka
