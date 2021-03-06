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

        bool Initialize(Device& device, RenderContext& renderContext) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;
    };

} // namespace Boolka
