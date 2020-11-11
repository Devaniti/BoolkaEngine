#include "stdafx.h"
#include "GraphicCommandAllocator.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    bool GraphicCommandAllocator::Initialize(Device& device)
    {
        ID3D12CommandAllocator* commandAllocator = nullptr;
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

        return CommandAllocator::Initialize(commandAllocator);
    }

    bool GraphicCommandAllocator::InitializeCommandList(GraphicCommandListImpl& commandList, Device& device, ID3D12PipelineState* PSO)
    {
        return commandList.Initialize(device, m_CommandAllocator, PSO);
    }

    bool GraphicCommandAllocator::ResetCommandList(GraphicCommandListImpl& commandList, ID3D12PipelineState* PSO)
    {
        return commandList.Reset(m_CommandAllocator, PSO);
    }

    void GraphicCommandAllocator::Unload()
    {
        CommandAllocator::Unload();
    }

}