#pragma once
#include "CommandAllocator.h"
#include "APIWrappers/CommandList/ComputeCommandListImpl.h"

namespace Boolka
{

    class ComputeCommandAllocator : public CommandAllocator
    {
    public:
        ComputeCommandAllocator() {};
        ~ComputeCommandAllocator() {};

        bool Initialize(Device& device);
        void Unload();

        bool InitializeCommandList(ComputeCommandListImpl& commandList, Device& device, ID3D12PipelineState* PSO);
        bool ResetCommandList(ComputeCommandListImpl& commandList, ID3D12PipelineState* PSO);
    };

}
