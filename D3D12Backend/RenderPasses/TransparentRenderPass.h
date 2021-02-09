#pragma once
#include "RenderPass.h"

#include "APIWrappers/PipelineState/GraphicPipelineState.h"

namespace Boolka
{

    class TransparentRenderPass :
        public RenderPass
    {
    public:
        TransparentRenderPass() = default;
        ~TransparentRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;
    };

}

