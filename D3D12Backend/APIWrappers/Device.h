#pragma once
#include "CommandQueue/ComputeQueue.h"
#include "CommandQueue/CopyQueue.h"
#include "CommandQueue/GraphicQueue.h"
#include "DirectStorage/DStorageFactory.h"
#include "DirectStorage/DStorageQueue.h"
#include "Factory.h"
#include "FeatureSupportHelper.h"

namespace Boolka
{

    struct RenderCacheContainer;

    class [[nodiscard]] Device
    {
    public:
        Device();
        ~Device();

        [[nodiscard]] ID3D12Device6* Get();
        [[nodiscard]] ID3D12Device6* operator->();

        [[nodiscard]] GraphicQueue& GetGraphicQueue();
        [[nodiscard]] ComputeQueue& GetComputeQueue();
        [[nodiscard]] CopyQueue& GetCopyQueue();
        [[nodiscard]] DStorageQueue& GetDStorageQueue();

        [[nodiscard]] DStorageFactory& GetDStorageFactory();

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

        DStorageFactory m_DStorageFactory;

        GraphicQueue m_GraphicQueue;
        ComputeQueue m_ComputeQueue;
        CopyQueue m_CopyQueue;
        DStorageQueue m_DStorageQueue;
        FeatureSupportHelper m_FeatureSupportHelper;
    };

} // namespace Boolka
