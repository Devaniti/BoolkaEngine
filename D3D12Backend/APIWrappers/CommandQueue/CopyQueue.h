#pragma once
#include "CommandQueue.h"

namespace Boolka
{

    class Device;
    class CopyCommandListImpl;

    class [[nodiscard]] CopyQueue : public CommandQueue
    {
    public:
        CopyQueue() = default;
        ~CopyQueue() = default;

        bool Initialize(Device& device);

        void ExecuteCommandList(CopyCommandListImpl& commandList);

    private:
    };

} // namespace Boolka
