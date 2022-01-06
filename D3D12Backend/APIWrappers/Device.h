#pragma once
#include "CommandQueue/ComputeQueue.h"
#include "CommandQueue/CopyQueue.h"
#include "CommandQueue/GraphicQueue.h"
#include "Factory.h"
#include "FeatureSupportHelper.h"
#include "PipelineState/PipelineStateLibrary.h"

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

#ifdef BLK_ENABLE_PIPELINE_LIBRARY
        [[nodiscard]] PipelineStateLibrary& GetPSOLibrary();
#endif

        bool Initialize(Factory& factory, RenderCacheContainer& renderCache);
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
        FeatureSupportHelper m_FeatureSupportHelper;

#ifdef BLK_ENABLE_PIPELINE_LIBRARY
        PipelineStateLibrary m_PSOLibrary;
#endif
    };

} // namespace Boolka
