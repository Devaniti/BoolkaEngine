#pragma once
#include "CommandQueue.h"

namespace Boolka
{

    class Device;
    class ComputeCommandListImpl;

    class [[nodiscard]] ComputeQueue : public CommandQueue
    {
    public:
        ComputeQueue() = default;
        ~ComputeQueue() = default;

        bool Initialize(Device& device);

        void ExecuteCommandList(ComputeCommandListImpl& commandList);

    private:
    };

} // namespace Boolka
