#include "stdafx.h"

#include "CopyCommandAllocator.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    bool CopyCommandAllocator::Initialize(Device& device)
    {
        ID3D12CommandAllocator* commandAllocator = nullptr;
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY,
                                       IID_PPV_ARGS(&commandAllocator));

        return CommandAllocator::Initialize(commandAllocator);
    }

    bool CopyCommandAllocator::InitializeCommandList(CopyCommandListImpl& commandList,
                                                     Device& device, ID3D12PipelineState* PSO)
    {
        return commandList.Initialize(device, m_CommandAllocator, PSO);
    }

    bool CopyCommandAllocator::ResetCommandList(CopyCommandListImpl& commandList,
                                                ID3D12PipelineState* PSO)
    {
        return commandList.Reset(m_CommandAllocator, PSO);
    }

    void CopyCommandAllocator::Unload()
    {
        CommandAllocator::Unload();
    }

} // namespace Boolka