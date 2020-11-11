#pragma once
#include "CommandQueue.h"

namespace Boolka
{

    class Device;
    class GraphicCommandListImpl;

    class GraphicQueue : public CommandQueue
    {
    public:
        GraphicQueue() {};
        ~GraphicQueue() {};

        bool Initialize(Device& device);

        void ExecuteCommandList(GraphicCommandListImpl& commandList);
    private:

    };

}

