#pragma once
#include "APIWrappers/CommandList/ComputeCommandListImpl.h"
#include "CommandAllocator.h"

namespace Boolka
{

    class [[nodiscard]] ComputeCommandAllocator : public CommandAllocator
    {
    public:
        ComputeCommandAllocator() = default;
        ~ComputeCommandAllocator() = default;

        bool Initialize(Device& device);
        void Unload();

        bool InitializeCommandList(ComputeCommandListImpl& commandList, Device& device,
                                   ID3D12PipelineState* PSO);
        bool ResetCommandList(ComputeCommandListImpl& commandList, ID3D12PipelineState* PSO);
    };

} // namespace Boolka
