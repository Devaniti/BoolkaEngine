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

        ID3D12CommandSignature* Get();
        ID3D12CommandSignature* operator->();

        bool Initialize(Device& device);
        void Unload();

    private:
        ID3D12CommandSignature* m_CommandSignature;
    };

} // namespace Boolka
