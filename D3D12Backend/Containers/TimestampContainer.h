#pragma once
#include "../APIWrappers/Queries/QueryHeap.h"

namespace Boolka
{
    // Temp container for lights
    class TimestampContainer
    {
    public:
        TimestampContainer();
        ~TimestampContainer();

        enum class Markers
        {
            UpdateRenderPass,
            BeginFrame = UpdateRenderPass,
            GPUCullingRenderPass,
            ZRenderPass,
            ShadowMapRenderPass,
            GBufferRenderPass,
            ReflectionRenderPass,
            DeferredLightingPass,
            SkyBoxRenderPass,
            TransparentRenderPass,
            ToneMappingPass,
            DebugOverlayPass,
            PresentPass,
            EndFrame,
            Count
        };

        bool Initialize(Device& device, GraphicCommandListImpl& m_InitializationCommandList);
        void Unload();

        void Mark(GraphicCommandListImpl& commandList, Markers marker);
        void FinishFrame(GraphicCommandListImpl& commandList, RenderContext& renderContext,
                         UINT frameIndex);

    private:
        QueryHeap m_queryHeap;
        ReadbackBuffer m_readbackBuffer[BLK_IN_FLIGHT_FRAMES];
        UINT64 m_timestampFrequency;
    };

} // namespace Boolka
