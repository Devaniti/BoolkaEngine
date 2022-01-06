#pragma once
#include "RenderPass.h"

namespace Boolka
{

    class [[nodiscard]] PresentPass : public RenderPass
    {
    public:
        PresentPass() = default;
        ~PresentPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
    };

} // namespace Boolka
