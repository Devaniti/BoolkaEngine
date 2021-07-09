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

        bool Initialize(Device& device, ID3D12RootSignature* rootSig, UINT commandStride,
                        UINT argumentCount, const D3D12_INDIRECT_ARGUMENT_DESC* arguments);
        void Unload();

    private:
        ID3D12CommandSignature* m_CommandSignature;
    };

} // namespace Boolka
