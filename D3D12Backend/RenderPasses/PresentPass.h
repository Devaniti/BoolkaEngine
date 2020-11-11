#pragma once
#include "RenderPass.h"
namespace Boolka
{

    class PresentPass :
        public RenderPass
    {
    public:
        PresentPass();
        ~PresentPass();

        bool Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
    };

}

