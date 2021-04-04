#pragma once
#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "RenderPass.h"

namespace Boolka
{

    class ToneMappingPass : public RenderPass
    {
    public:
        ToneMappingPass() = default;
        ~ToneMappingPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
        GraphicPipelineState m_PSO;
    };

} // namespace Boolka
