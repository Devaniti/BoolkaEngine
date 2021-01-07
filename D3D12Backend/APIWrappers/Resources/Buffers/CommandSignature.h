#pragma once

namespace Boolka
{

    class Device;
    class RootSignature;

    class CommandSignature
    {
    public:
        CommandSignature();
        ~CommandSignature();

        ID3D12CommandSignature* Get() { BLK_ASSERT(m_CommandSignature != nullptr); return m_CommandSignature; }
        ID3D12CommandSignature* operator->() { return Get(); }

        bool Initialize(Device& device);
        void Unload();

    private:
        ID3D12CommandSignature* m_CommandSignature;
    };

}
