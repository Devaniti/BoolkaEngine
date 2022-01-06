#pragma once
#include "GraphicCommandList.h"

namespace Boolka
{
    class Device;

    class [[nodiscard]] GraphicCommandListImpl : public GraphicCommandList
    {
    public:
        GraphicCommandListImpl() = default;
        ~GraphicCommandListImpl() = default;

        bool Initialize(Device& device, ID3D12CommandAllocator* allocator,
                        ID3D12PipelineState* PSO);
        bool Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO);
    };

} // namespace Boolka
