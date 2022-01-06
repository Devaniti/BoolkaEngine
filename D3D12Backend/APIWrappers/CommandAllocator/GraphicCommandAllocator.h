#pragma once
#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "CommandAllocator.h"

namespace Boolka
{
    class Device;

    class [[nodiscard]] GraphicCommandAllocator : public CommandAllocator
    {
    public:
        GraphicCommandAllocator() = default;
        ~GraphicCommandAllocator() = default;

        bool Initialize(Device& device);
        void Unload();

        bool InitializeCommandList(GraphicCommandListImpl& commandList, Device& device,
                                   ID3D12PipelineState* PSO);
        bool ResetCommandList(GraphicCommandListImpl& commandList, ID3D12PipelineState* PSO);
    };

} // namespace Boolka
