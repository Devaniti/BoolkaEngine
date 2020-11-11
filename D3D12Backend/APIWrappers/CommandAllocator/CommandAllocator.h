#pragma once

namespace Boolka
{

    class CommandAllocator
    {
    public:
        bool Reset();

        ID3D12CommandAllocator* Get() { BLK_ASSERT(m_CommandAllocator != nullptr); return m_CommandAllocator; };
        ID3D12CommandAllocator* operator->() { return Get(); };

    protected:
        CommandAllocator();
        ~CommandAllocator();

        CommandAllocator(const CommandAllocator&) = delete;
        CommandAllocator(CommandAllocator&&) = delete;
        CommandAllocator& operator=(const CommandAllocator&) = delete;
        CommandAllocator& operator=(CommandAllocator&&) = delete;

        bool Initialize(ID3D12CommandAllocator* commandAllocator);
        void Unload();

        ID3D12CommandAllocator* m_CommandAllocator;
    };

}
