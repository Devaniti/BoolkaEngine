#pragma once
#include "CommandQueue.h"

namespace Boolka
{

    class Device;
    class GraphicCommandListImpl;

    class GraphicQueue : public CommandQueue
    {
    public:
        GraphicQueue() = default;
        ~GraphicQueue() = default;

        bool Initialize(Device& device);

        void ExecuteCommandList(GraphicCommandListImpl& commandList);

    private:
    };

} // namespace Boolka
