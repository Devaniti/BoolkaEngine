#pragma once
#include "CommandQueue.h"

namespace Boolka
{

    class Device;

    class ComputeQueue : public CommandQueue
    {
    public:
        ComputeQueue() {};
        ~ComputeQueue() {};

        bool Initialize(Device& device);
    private:

    };

}

