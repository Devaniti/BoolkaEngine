#pragma once
#include "ComputeCommandList.h"

namespace Boolka
{

    class Device;

    class ComputeCommandListImpl : public ComputeCommandList
    {
    public:
        ComputeCommandListImpl() {};
        ~ComputeCommandListImpl() {};

        bool Initialize(Device& device, ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO);
        bool Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO);
    };

}
