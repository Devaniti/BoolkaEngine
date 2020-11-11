#include "stdafx.h"
#include "CopyQueue.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/CommandList/CopyCommandListImpl.h"

namespace Boolka
{

    bool CopyQueue::Initialize(Device& device)
    {
        ID3D12CommandQueue* commandQueue = nullptr;
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;
        HRESULT hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue));
        if (FAILED(hr)) return false;

        return CommandQueue::Initialize(device, commandQueue);
    }

    void CopyQueue::ExecuteCommandList(CopyCommandListImpl& commandList)
    {
        ID3D12CommandList* nativeCommandList = commandList.Get();
        m_Queue->ExecuteCommandLists(1, &nativeCommandList);
    }

}
