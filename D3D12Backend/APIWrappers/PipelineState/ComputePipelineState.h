#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"
#include "PipelineState.h"

namespace Boolka
{
    class Device;
    class RootSignature;

    class ComputePipelineState : public PipelineState
    {
    public:
        ComputePipelineState() = default;
        ~ComputePipelineState() = default;

        bool Initialize(Device& device, RootSignature& rootSig,
                        const MemoryBlock& computeShaderBytecode);
    };

} // namespace Boolka
