#pragma once
#include "RenderPass.h"

// Use this file as reference when creating new render passes

namespace Boolka
{

    class ReferenceRenderPass : public RenderPass
    {
    public:
        ReferenceRenderPass() = default;
        ~ReferenceRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
    };

} // namespace Boolka
