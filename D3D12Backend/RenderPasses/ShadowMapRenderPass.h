#pragma once
#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "RenderPass.h"

namespace Boolka
{

    class ShadowMapRenderPass : public RenderPass
    {
    public:
        ShadowMapRenderPass() = default;
        ~ShadowMapRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
        GraphicPipelineState m_PSO;
    };

} // namespace Boolka
