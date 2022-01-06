#pragma once

namespace Boolka
{

    class Device;

    class [[nodiscard]] RootSignature
    {
    public:
        RootSignature();
        ~RootSignature();

        [[nodiscard]] ID3D12RootSignature* Get() const;
        [[nodiscard]] ID3D12RootSignature* operator->() const;

        bool Initialize(Device& device, const char* filename);
        void Unload();

    private:
        ID3D12RootSignature* m_RootSignature;
    };

} // namespace Boolka
