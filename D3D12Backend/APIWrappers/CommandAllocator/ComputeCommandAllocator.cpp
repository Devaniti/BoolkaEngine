#include "stdafx.h"

#include "ComputeCommandAllocator.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    bool ComputeCommandAllocator::Initialize(Device& device)
    {
        ID3D12CommandAllocator* commandAllocator = nullptr;
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE,
                                       IID_PPV_ARGS(&commandAllocator));

        return CommandAllocator::Initialize(commandAllocator);
    }

    bool ComputeCommandAllocator::InitializeCommandList(ComputeCommandListImpl& commandList,
                                                        Device& device, ID3D12PipelineState* PSO)
    {
        return commandList.Initialize(device, m_CommandAllocator, PSO);
    }

    bool ComputeCommandAllocator::ResetCommandList(ComputeCommandListImpl& commandList,
                                                   ID3D12PipelineState* PSO)
    {
        return commandList.Reset(m_CommandAllocator, PSO);
    }

    void ComputeCommandAllocator::Unload()
    {
        CommandAllocator::Unload();
    }

} // namespace Boolka