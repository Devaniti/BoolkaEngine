#include "stdafx.h"
#include "Device.h"

namespace Boolka
{

    Device::Device()
        : m_Device(nullptr)
    {
    }

    Device::~Device()
    {
        BLK_ASSERT(m_Device == nullptr);
    }

    bool Device::Initialize(Factory& factory)
    {
        BLK_ASSERT(m_Device == nullptr);

        HRESULT hr = ::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
        if (FAILED(hr)) return false;

        BLK_RENDER_DEBUG_ONLY(InitializeDebug());

        if (!m_FeatureSupportHelper.Initialize(*this)) return false;

        if (!m_GraphicQueue.Initialize(*this)) return false;
        if (!m_ComputeQueue.Initialize(*this)) return false;
        if (!m_CopyQueue.Initialize(*this)) return false;

        if (!m_StateManager.Initialize(*this)) return false;

        return true;
    }

    void Device::Unload()
    {
        BLK_ASSERT(m_Device != nullptr);

        m_StateManager.Unload();

        m_CopyQueue.Unload();
        m_ComputeQueue.Unload();
        m_GraphicQueue.Unload();

        m_FeatureSupportHelper.Unload();

        BLK_RENDER_DEBUG_ONLY(ReportObjectLeaks());

        m_Device->Release();
        m_Device = nullptr;
    }

    void Device::Flush()
    {
        Fence* fences[3] =
        {
            &m_GraphicQueue.GetFence(),
            &m_ComputeQueue.GetFence(),
            &m_CopyQueue.GetFence()
        };

        UINT64 values[3] =
        {
            m_GraphicQueue.SignalGPU(),
            m_ComputeQueue.SignalGPU(),
            m_CopyQueue.SignalGPU()
        };

        Fence::WaitCPUMultiple(3, fences, values);
    }

#ifdef BLK_RENDER_DEBUG
    void Device::InitializeDebug()
    {
        SetDebugBreakSeverity(D3D12_MESSAGE_SEVERITY_WARNING);
    }

    void Device::SetDebugBreakSeverity(D3D12_MESSAGE_SEVERITY severity)
    {
        ID3D12InfoQueue* debugInfoQueue = nullptr;
        HRESULT hr = m_Device->QueryInterface(IID_PPV_ARGS(&debugInfoQueue));
        if (FAILED(hr))
        {
            return;
        }

        for (int i = 0; i <= D3D12_MESSAGE_SEVERITY_MESSAGE; i++)
        {
            // lower enum value corresponds to higher severity
            bool needBreak = (i <= severity);
            debugInfoQueue->SetBreakOnSeverity(static_cast<D3D12_MESSAGE_SEVERITY>(i), needBreak);
        }
        debugInfoQueue->Release();
    }

    void Device::ReportObjectLeaks()
    {
        ID3D12DebugDevice2* debugDevice = nullptr;
        HRESULT hr = m_Device->QueryInterface(IID_PPV_ARGS(&debugDevice));
        if (FAILED(hr))
        {
            return;
        }

        D3D12_MESSAGE_ID filterList[] = { D3D12_MESSAGE_ID_LIVE_OBJECT_SUMMARY, D3D12_MESSAGE_ID_LIVE_DEVICE };
        FilterMessage(filterList, ARRAYSIZE(filterList));
        // debugDevice->ReportLiveDeviceObjects always report reference to device, since debugDevice itself is reference to device
        debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
        RemoveLastMessageFilter();
        ULONG refCount = debugDevice->Release();
        // Instead of relying on debug layer warnings we check ref count manually
        // After releasing debugDevice there should only be single reference left that we'll release right after Device::ReportObjectLeaks call
        BLK_ASSERT(refCount == 1);
    }

    void Device::FilterMessage(D3D12_MESSAGE_ID id)
    {
        FilterMessage(&id, 1);
    }

    void Device::FilterMessage(D3D12_MESSAGE_ID* idArray, UINT idCount)
    {
        ID3D12InfoQueue* debugInfoQueue = nullptr;
        HRESULT hr = m_Device->QueryInterface(IID_PPV_ARGS(&debugInfoQueue));
        if (FAILED(hr))
        {
            return;
        }

        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = idCount;
        filter.DenyList.pIDList = idArray;
        hr = debugInfoQueue->PushStorageFilter(&filter);
        BLK_ASSERT(SUCCEEDED(hr));

        debugInfoQueue->Release();
    }

    void Device::RemoveLastMessageFilter()
    {
        ID3D12InfoQueue* debugInfoQueue = nullptr;
        HRESULT hr = m_Device->QueryInterface(IID_PPV_ARGS(&debugInfoQueue));
        if (FAILED(hr))
        {
            return;
        }
        debugInfoQueue->PopStorageFilter();

        debugInfoQueue->Release();
    }

    #endif

}
