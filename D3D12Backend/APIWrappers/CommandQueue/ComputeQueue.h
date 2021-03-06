#pragma once
#include "CommandQueue.h"

namespace Boolka
{

    class Device;

    class ComputeQueue : public CommandQueue
    {
    public:
        ComputeQueue() = default;
        ~ComputeQueue() = default;

        bool Initialize(Device& device);

    private:
    };

} // namespace Boolka
