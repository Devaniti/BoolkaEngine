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
        ID3D12GraphicsCommandList5* Get() { BLK_ASSERT(m_CommandList != nullptr); return m_CommandList; };
        ID3D12GraphicsCommandList5* operator->() { return Get(); };

        void Unload();

    protected:
        bool Initialize(ID3D12GraphicsCommandList5* commandList);

        ID3D12GraphicsCommandList5* m_CommandList;
    };

}
