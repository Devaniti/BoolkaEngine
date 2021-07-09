#pragma once
#include "CommandQueue/ComputeQueue.h"
#include "CommandQueue/CopyQueue.h"
#include "CommandQueue/GraphicQueue.h"
#include "Factory.h"
#include "FeatureSupportHelper.h"
#include "StateManager.h"

namespace Boolka
{

    class Device
    {
    public:
        Device();
        ~Device();

        ID3D12Device6* Get();
        ID3D12Device6* operator->();

        GraphicQueue& GetGraphicQueue();
        ComputeQueue& GetComputeQueue();
        CopyQueue& GetCopyQueue();

        bool Initialize(Factory& factory);
        void Unload();

        void Flush();

        void CheckIsDeviceAlive();

#ifdef BLK_RENDER_DEBUG
        void FilterMessage(D3D12_MESSAGE_ID id);
        void FilterMessage(D3D12_MESSAGE_ID* idArray, UINT idCount);
        void RemoveLastMessageFilter();
#endif
    private:
#ifdef BLK_RENDER_PROFILING
        void InitializeProfiling();
#endif
#ifdef BLK_RENDER_DEBUG
        void InitializeDebug();
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

} // namespace Boolka
