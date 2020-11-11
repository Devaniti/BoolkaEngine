#pragma once
#include "CommandAllocator.h"
#include "APIWrappers/CommandList/GraphicCommandListImpl.h"

namespace Boolka
{
    class Device;

    class GraphicCommandAllocator : public CommandAllocator
    {
    public:
        GraphicCommandAllocator() {};
        ~GraphicCommandAllocator() {};

        bool Initialize(Device& device);
        void Unload();

        bool InitializeCommandList(GraphicCommandListImpl& commandList, Device& device, ID3D12PipelineState* PSO);
        bool ResetCommandList(GraphicCommandListImpl& commandList, ID3D12PipelineState* PSO);
    };

}
