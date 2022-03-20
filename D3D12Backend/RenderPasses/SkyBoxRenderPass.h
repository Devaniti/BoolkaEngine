#pragma once
#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "RenderPass.h"

// Use this file as reference when creating new render passes

namespace Boolka
{

    class [[nodiscard]] SkyBoxRenderPass : public RenderPass
    {
    public:
        SkyBoxRenderPass() = default;
        ~SkyBoxRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
    };

} // namespace Boolka
