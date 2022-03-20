#pragma once
#include "../APIWrappers/Queries/QueryHeap.h"

namespace Boolka
{
    // Temp container for timestamps
    class [[nodiscard]] TimestampContainer
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
            RaytraceRenderPass,
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
        QueryHeap m_QueryHeap;
        ReadbackBuffer m_ReadbackBuffer[BLK_IN_FLIGHT_FRAMES];
        UINT64 m_TimestampFrequency;
    };

} // namespace Boolka
