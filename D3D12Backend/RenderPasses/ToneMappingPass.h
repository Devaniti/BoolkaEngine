#pragma once
#include "RenderPass.h"
#include "APIWrappers/PipelineState/GraphicPipelineState.h"

namespace Boolka
{

    class ToneMappingPass :
        public RenderPass
    {
    public:
        ToneMappingPass() = default;
        ~ToneMappingPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;
    };

}
