#pragma once
#include "CommandQueue.h"

namespace Boolka
{

    class Device;
    class CopyCommandListImpl;

    class CopyQueue : public CommandQueue
    {
    public:
        CopyQueue() {};
        ~CopyQueue() {};

        bool Initialize(Device& device);

        void ExecuteCommandList(CopyCommandListImpl& commandList);
    private:

    };

}

