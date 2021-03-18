#include "stdafx.h"

#include "Fence.h"

#include "Device.h"

namespace Boolka
{

    Fence::Fence()
        : m_Fence(nullptr)
        , m_ExpactedValue(0)
        , m_CPUEvent(NULL)
    {
    }

    Fence::~Fence()
    {
        BLK_ASSERT(m_Fence == nullptr);
        BLK_ASSERT(m_CPUEvent == NULL);
    }

    bool Fence::Initialize(Device& device)
    {
        m_ExpactedValue = 0;
        HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        if (FAILED(hr))
            return false;

        m_CPUEvent = ::CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
        if (m_CPUEvent == NULL)
            return false;

        return true;
    }

    void Fence::Unload()
    {
        BLK_ASSERT(m_Fence != nullptr);
        BLK_ASSERT(m_CPUEvent != NULL);

        m_Fence->Release();
        m_Fence = nullptr;

        ::CloseHandle(m_CPUEvent);
        m_CPUEvent = NULL;
    }

    UINT64 Fence::SignalCPU()
    {
        m_Fence->Signal(++m_ExpactedValue);
        return m_ExpactedValue;
    }

    UINT64 Fence::SignalGPU(CommandQueue& commandQueue)
    {
        commandQueue->Signal(m_Fence, ++m_ExpactedValue);
        return m_ExpactedValue;
    }

    void Fence::SignalCPUWithValue(UINT64 value)
    {
        m_ExpactedValue = value;
        m_Fence->Signal(value);
    }

    void Fence::SignalGPUWithValue(UINT64 value, CommandQueue& commandQueue)
    {
        m_ExpactedValue = value;
        commandQueue->Signal(m_Fence, value);
    }

    void Fence::WaitCPU(UINT64 value)
    {
        HRESULT hr = m_Fence->SetEventOnCompletion(value, m_CPUEvent);
        BLK_ASSERT_VAR(SUCCEEDED(hr));
        ::WaitForSingleObject(m_CPUEvent, INFINITE);
        ::ResetEvent(m_CPUEvent);
    }

    void Fence::WaitGPU(UINT64 value, CommandQueue& commandQueue)
    {
        commandQueue->Wait(m_Fence, value);
    }

    void Fence::WaitCPUMultiple(size_t count, Fence** fences, UINT64* values)
    {
        BLK_ASSERT(count <= 16);
        HANDLE cpuEvents[16];

        for (size_t i = 0; i < count; i++)
        {
            HRESULT hr = fences[i]->m_Fence->SetEventOnCompletion(values[i], fences[i]->m_CPUEvent);
            BLK_ASSERT_VAR(SUCCEEDED(hr));
            cpuEvents[i] = fences[i]->m_CPUEvent;
        }

        // TODO replace with define for max DWORD value
        BLK_ASSERT(count <= static_cast<DWORD>(-1));

        ::WaitForMultipleObjects(static_cast<DWORD>(count), cpuEvents, TRUE, INFINITE);

        for (size_t i = 0; i < count; i++)
        {
            ::ResetEvent(fences[i]->m_CPUEvent);
        }
    }

} // namespace Boolka
