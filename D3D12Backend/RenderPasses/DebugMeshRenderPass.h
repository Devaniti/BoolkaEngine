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

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
        GraphicPipelineState m_PSO;

        float m_CurrentAngle;
    };

} // namespace Boolka
