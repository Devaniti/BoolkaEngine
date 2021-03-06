#pragma once
#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "RenderPass.h"

namespace Boolka
{

    class TransparentRenderPass : public RenderPass
    {
    public:
        TransparentRenderPass() = default;
        ~TransparentRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;
    };

} // namespace Boolka
