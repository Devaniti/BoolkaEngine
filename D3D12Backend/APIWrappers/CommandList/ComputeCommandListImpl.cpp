#include "stdafx.h"

#include "ComputeCommandListImpl.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    bool ComputeCommandListImpl::Initialize(Device& device, ID3D12CommandAllocator* allocator,
                                            ID3D12PipelineState* PSO)
    {
        ID3D12GraphicsCommandList6* commandList = nullptr;
        HRESULT hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, allocator, PSO,
                                               IID_PPV_ARGS(&commandList));
        if (FAILED(hr))
            return false;

        return CommandList::Initialize(commandList);
    }

    bool ComputeCommandListImpl::Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO)
    {
        HRESULT hr = m_CommandList->Reset(allocator, PSO);
        return SUCCEEDED(hr);
    }

} // namespace Boolka