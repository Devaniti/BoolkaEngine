#pragma once
#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "Camera.h"
#include "RenderPass.h"

namespace Boolka
{

    class [[nodiscard]] GBufferRenderPass : public RenderPass
    {
    public:
        GBufferRenderPass() = default;
        ~GBufferRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
    };

} // namespace Boolka
