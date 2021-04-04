#pragma once
#include "RenderPass.h"

namespace Boolka
{

    class UpdateRenderPass : public RenderPass
    {
    public:
        UpdateRenderPass() = default;
        ~UpdateRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
    };

} // namespace Boolka
