#pragma once
#include "Factory.h"
#include "CommandQueue/GraphicQueue.h"
#include "CommandQueue/ComputeQueue.h"
#include "CommandQueue/CopyQueue.h"
#include "FeatureSupportHelper.h"
#include "StateManager.h"

namespace Boolka
{

    class Device
    {
    public:
        Device();
        ~Device();

        ID3D12Device6* Get() { BLK_ASSERT(m_Device != nullptr); return m_Device; };
        ID3D12Device6* operator->() { return Get(); };

        GraphicQueue& GetGraphicQueue() { return m_GraphicQueue; }
        ComputeQueue& GetComputeQueue() { return m_ComputeQueue; }
        CopyQueue& GetCopyQueue() { return m_CopyQueue; }

        bool Initialize(Factory& factory);
        void Unload();

        void Flush();
    private:
#ifdef BLK_RENDER_DEBUG
        void SetDebugBreakSeverity(D3D12_MESSAGE_SEVERITY severity);
        void ReportObjectLeaks();
#endif
        ID3D12Device6* m_Device;

        GraphicQueue m_GraphicQueue;
        ComputeQueue m_ComputeQueue;
        CopyQueue m_CopyQueue;
        StateManager m_StateManager;
        FeatureSupportHelper m_FeatureSupportHelper;
    };

}
