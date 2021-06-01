#pragma once

namespace Boolka
{

    class Device;

    class BottomLevelAS
    {
    public:
        static void GetSizes(Device& device, UINT vertexCount, UINT vertexStride, UINT indexCount,
                             UINT64& outScratchSize, UINT64& outBLASSize);
        static void Initialize(ComputeCommandList& commandList,
                               D3D12_GPU_VIRTUAL_ADDRESS destination,
                               D3D12_GPU_VIRTUAL_ADDRESS scratchBuffer,
                               D3D12_GPU_VIRTUAL_ADDRESS vertexBuffer, UINT vertexCount,
                               UINT vertexStride, D3D12_GPU_VIRTUAL_ADDRESS indexBuffer,
                               UINT indexCount,
                               D3D12_GPU_VIRTUAL_ADDRESS postBuildDataBuffer = NULL);
    };

} // namespace Boolka
