#pragma once
#include "GraphicCommandList.h"

namespace Boolka
{
    class Device;

    class GraphicCommandListImpl : public GraphicCommandList
    {
    public:
        GraphicCommandListImpl() {};
        ~GraphicCommandListImpl() {};

        bool Initialize(Device& device, ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO);
        bool Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO);
    };

}
