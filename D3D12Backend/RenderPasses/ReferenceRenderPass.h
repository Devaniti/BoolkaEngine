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

        bool Initialize(Device& device, RenderContext& renderContext) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
    };

} // namespace Boolka
