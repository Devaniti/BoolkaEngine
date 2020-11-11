#include "stdafx.h"
#include "GraphicCommandListImpl.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    bool GraphicCommandListImpl::Initialize(Device& device, ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO)
    {
        ID3D12GraphicsCommandList5* commandList = nullptr;
        HRESULT hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, PSO, IID_PPV_ARGS(&commandList));
        if (FAILED(hr)) return false;

        return CommandList::Initialize(commandList);
    }

    bool GraphicCommandListImpl::Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* PSO)
    {
        HRESULT hr = m_CommandList->Reset(allocator, PSO);
        return SUCCEEDED(hr);
    }

}