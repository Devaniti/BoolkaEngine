#pragma once

namespace Boolka
{

    class [[nodiscard]] CommandList
    {
    protected:
        CommandList();
        ~CommandList();

        CommandList(const CommandList&) = delete;
        CommandList(CommandList&&) = delete;
        CommandList& operator=(const CommandList&) = delete;
        CommandList& operator=(CommandList&&) = delete;

    public:
        [[nodiscard]] ID3D12GraphicsCommandList6* Get();
        [[nodiscard]] ID3D12GraphicsCommandList6* operator->();

        void Unload();

    protected:
        bool Initialize(ID3D12GraphicsCommandList6* commandList);

        ID3D12GraphicsCommandList6* m_CommandList;
    };

} // namespace Boolka
