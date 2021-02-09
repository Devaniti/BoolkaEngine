#pragma once
#include "RenderPass.h"

#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "Camera.h"

namespace Boolka
{

    class GBufferRenderPass :
        public RenderPass
    {
    public:
        GBufferRenderPass() = default;
        ~GBufferRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;
    };

}

