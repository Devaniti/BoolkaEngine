#pragma once

namespace Boolka
{

    class Device;

    class TopLevelAS
    {
    public:
        static void GetSizes(Device& device, UINT blasInstanceCount, UINT64& outScratchSize,
                             UINT64& outTLASSize);
        static void Initialize(ComputeCommandList& commandList,
                               D3D12_GPU_VIRTUAL_ADDRESS destination,
                               D3D12_GPU_VIRTUAL_ADDRESS scratchBuffer,
                               D3D12_GPU_VIRTUAL_ADDRESS blasInstances, UINT instanceCount);
    };

} // namespace Boolka
