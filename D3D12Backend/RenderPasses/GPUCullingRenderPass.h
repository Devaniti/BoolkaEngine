#pragma once
#include "APIWrappers/PipelineState/ComputePipelineState.h"
#include "RenderPass.h"

namespace Boolka
{

    class [[nodiscard]] GPUCullingRenderPass : public RenderPass
    {
    public:
        GPUCullingRenderPass() = default;
        ~GPUCullingRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
#ifdef BLK_ENABLE_STATS
        ReadbackBuffer m_CulledCountBuffer[BLK_IN_FLIGHT_FRAMES];
#endif
    };

} // namespace Boolka
