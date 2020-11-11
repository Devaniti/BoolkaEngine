#pragma once
#include "CommandAllocator.h"
#include "APIWrappers/CommandList/CopyCommandListImpl.h"

namespace Boolka
{
    class Device;

    class CopyCommandAllocator : public CommandAllocator
    {
    public:
        CopyCommandAllocator() {};
        ~CopyCommandAllocator() {};

        bool Initialize(Device& device);
        void Unload();

        bool InitializeCommandList(CopyCommandListImpl& commandList, Device& device, ID3D12PipelineState* PSO);
        bool ResetCommandList(CopyCommandListImpl& commandList, ID3D12PipelineState* PSO);
    };

}
