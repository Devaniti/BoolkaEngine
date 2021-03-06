#pragma once

namespace Boolka
{

    class CommandList
    {
    protected:
        CommandList();
        ~CommandList();

        CommandList(const CommandList&) = delete;
        CommandList(CommandList&&) = delete;
        CommandList& operator=(const CommandList&) = delete;
        CommandList& operator=(CommandList&&) = delete;

    public:
        ID3D12GraphicsCommandList5* Get();
        ID3D12GraphicsCommandList5* operator->();

        void Unload();

    protected:
        bool Initialize(ID3D12GraphicsCommandList5* commandList);

        ID3D12GraphicsCommandList5* m_CommandList;
    };

} // namespace Boolka
