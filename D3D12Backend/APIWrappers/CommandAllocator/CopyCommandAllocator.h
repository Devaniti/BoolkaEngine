#pragma once
#include "APIWrappers/CommandList/CopyCommandListImpl.h"
#include "CommandAllocator.h"

namespace Boolka
{
    class Device;

    class [[nodiscard]] CopyCommandAllocator : public CommandAllocator
    {
    public:
        CopyCommandAllocator() = default;
        ~CopyCommandAllocator() = default;

        bool Initialize(Device& device);
        void Unload();

        bool InitializeCommandList(CopyCommandListImpl& commandList, Device& device,
                                   ID3D12PipelineState* PSO);
        bool ResetCommandList(CopyCommandListImpl& commandList, ID3D12PipelineState* PSO);
    };

} // namespace Boolka
