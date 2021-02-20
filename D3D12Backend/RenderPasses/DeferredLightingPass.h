#pragma once
#include "RenderPass.h"
#include "APIWrappers/PipelineState/GraphicPipelineState.h"

namespace Boolka
{

    class DeferredLightingPass :
        public RenderPass
    {
    public:
        DeferredLightingPass() = default;
        ~DeferredLightingPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;
        Buffer m_ConstantBuffer[BLK_IN_FLIGHT_FRAMES];
        UploadBuffer m_ConstantUploadBuffer[BLK_IN_FLIGHT_FRAMES];
        float m_CurrentRotation;
    };

}
