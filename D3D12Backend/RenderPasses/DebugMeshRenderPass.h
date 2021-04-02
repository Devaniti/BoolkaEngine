#pragma once
#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "RenderPass.h"

namespace Boolka
{

    class DebugMeshRenderPass : public RenderPass
    {
    public:
        DebugMeshRenderPass();
        ~DebugMeshRenderPass();

        bool Initialize(Device& device, RenderContext& renderContext) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;

        float m_CurrentAngle;
    };

} // namespace Boolka
