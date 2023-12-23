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

        [[nodiscard]] bool SupportsRaytracing() const;

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
        bool SelectAndCreateDevice(Factory& factory);
        void InitializeFeatureSupport();
#ifdef BLK_RENDER_PROFILING
        void InitializeProfiling();
#endif
#ifdef BLK_RENDER_DEBUG
        void InitializeDebug();
        void SetDebugBreakSeverity(D3D12_MESSAGE_SEVERITY severity);
        void SetGPUBasedValidationShaderPatchMode(
            D3D12_GPU_BASED_VALIDATION_SHADER_PATCH_MODE patchMode);
        void ReportDeviceObjectLeaks();
#endif

        IDXGIAdapter4* m_Adapter;
        ID3D12Device6* m_Device;

        DStorageFactory m_DStorageFactory;

        GraphicQueue m_GraphicQueue;
        ComputeQueue m_ComputeQueue;
        CopyQueue m_CopyQueue;
        DStorageQueue m_DStorageQueue;

        bool m_SupportsRaytracing;
    };

} // namespace Boolka
