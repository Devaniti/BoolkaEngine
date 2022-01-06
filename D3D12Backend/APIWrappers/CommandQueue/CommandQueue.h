#pragma once

#include "APIWrappers/Fence.h"

namespace Boolka
{

    class Device;

    class [[nodiscard]] CommandQueue
    {
    public:
        [[nodiscard]] ID3D12CommandQueue* Get();
        [[nodiscard]] ID3D12CommandQueue* operator->();

        void Unload();

        [[nodiscard]] UINT64 SignalCPU();
        [[nodiscard]] UINT64 SignalGPU();
        void WaitCPU(UINT64 value);
        void WaitGPU(UINT64 value);

        void Flush();

        [[nodiscard]] Fence& GetFence();

    protected:
        CommandQueue();
        ~CommandQueue();

        CommandQueue(const CommandQueue&) = delete;
        CommandQueue(CommandQueue&&) = delete;
        CommandQueue& operator=(const CommandQueue&) = delete;
        CommandQueue& operator=(CommandQueue&&) = delete;

        bool Initialize(Device& device, ID3D12CommandQueue* queue);

        ID3D12CommandQueue* m_Queue;
        Fence m_Fence;
    };

} // namespace Boolka
