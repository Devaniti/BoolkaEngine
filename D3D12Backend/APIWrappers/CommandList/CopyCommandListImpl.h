#pragma once
#include "CopyCommandList.h"

namespace Boolka
{

    class Device;
    class [[nodiscard]] CopyCommandListImpl : public CopyCommandList
    {
    public:
        CopyCommandListImpl() = default;
        ~CopyCommandListImpl() = default;

        bool Initialize(Device& device, ID3D12CommandAllocator* allocator,
                        ID3D12PipelineState* PSO);
        bool Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO);
    };

} // namespace Boolka
