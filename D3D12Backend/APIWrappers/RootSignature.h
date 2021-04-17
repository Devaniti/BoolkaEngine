#pragma once

namespace Boolka
{
    class Device;

    class RootSignature
    {
    public:
        RootSignature();
        ~RootSignature();

        ID3D12RootSignature* Get() const;
        ID3D12RootSignature* operator->() const;

        bool Initialize(Device& device, const char* filename);
        void Unload();

    private:
        ID3D12RootSignature* m_RootSignature;
    };

} // namespace Boolka
