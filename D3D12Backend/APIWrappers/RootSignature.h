#pragma once

namespace Boolka
{
    class Device;

    class RootSignature
    {
    public:
        RootSignature();
        ~RootSignature();

        ID3D12RootSignature* Get() { BLK_ASSERT(m_RootSignature != nullptr); return m_RootSignature; };
        ID3D12RootSignature* operator->() { return Get(); };

        bool Initialize(Device& device, const char* filename);
        void Unload();
    private:
        ID3D12RootSignature* m_RootSignature;
    };

}
