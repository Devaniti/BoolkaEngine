#pragma once
#include "CopyCommandList.h"

namespace Boolka
{

    class Device;
    class CopyCommandListImpl :
        public CopyCommandList
    {
    public:
        CopyCommandListImpl() {};
        ~CopyCommandListImpl() {};

        bool Initialize(Device& device, ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO);
        bool Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO);
    };

}

