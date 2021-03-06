#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "RenderPass.h"

#ifdef BLK_ENABLE_STATS

namespace Boolka
{

    class DebugOverlayPass : public RenderPass
    {
    public:
        DebugOverlayPass() = default;
        ~DebugOverlayPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        void ImguiFlipFrame();
        void ImguiUIManagement(const RenderContext& renderContext);

        DescriptorHeap m_ImguiDescriptorHeap;
    };

} // namespace Boolka

#endif
