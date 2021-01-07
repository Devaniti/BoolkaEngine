#pragma once
#include "RenderPass.h"
namespace Boolka
{

    class UpdateRenderPass :
        public RenderPass
    {
    public:
        UpdateRenderPass() = default;
        ~UpdateRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
    };

}

