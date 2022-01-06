#pragma once

namespace Boolka
{

    class [[nodiscard]] CommandAllocator
    {
    public:
        bool Reset();

        [[nodiscard]] ID3D12CommandAllocator* Get();
        [[nodiscard]] ID3D12CommandAllocator* operator->();

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
