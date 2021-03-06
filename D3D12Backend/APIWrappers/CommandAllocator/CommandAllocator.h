#pragma once

namespace Boolka
{

    class CommandAllocator
    {
    public:
        bool Reset();

        ID3D12CommandAllocator* Get();
        ID3D12CommandAllocator* operator->();

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

} // namespace Boolka
