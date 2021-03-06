#include "stdafx.h"

#include "CommandQueue.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    CommandQueue::CommandQueue()
        : m_Queue(nullptr)
    {
    }

    CommandQueue::~CommandQueue()
    {
        BLK_ASSERT(m_Queue == nullptr);
    }

    bool CommandQueue::Initialize(Device& device, ID3D12CommandQueue* queue)
    {
        BLK_ASSERT(m_Queue == nullptr);

        m_Queue = queue;

        m_Fence.Initialize(device);

        return true;
    }

    ID3D12CommandQueue* CommandQueue::Get()
    {
        BLK_ASSERT(m_Queue != nullptr);
        return m_Queue;
    }

    ID3D12CommandQueue* CommandQueue::operator->()
    {
        return Get();
    }

    void CommandQueue::Unload()
    {
        BLK_ASSERT(m_Queue != nullptr);

        m_Fence.Unload();

        m_Queue->Release();
        m_Queue = nullptr;
    }

    UINT64 CommandQueue::SignalCPU()
    {
        return m_Fence.SignalCPU();
    }

    UINT64 CommandQueue::SignalGPU()
    {
        return m_Fence.SignalGPU(*this);
    }

    void CommandQueue::WaitCPU(UINT64 value)
    {
        m_Fence.WaitCPU(value);
    }

    void CommandQueue::WaitGPU(UINT64 value)
    {
        m_Fence.WaitGPU(value, *this);
    }

    void CommandQueue::Flush()
    {
        UINT64 value = SignalGPU();
        WaitCPU(value);
    }

    Fence& CommandQueue::GetFence()
    {
        return m_Fence;
    }

} // namespace Boolka
