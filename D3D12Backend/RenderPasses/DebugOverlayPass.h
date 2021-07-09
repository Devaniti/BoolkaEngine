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

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
        void ImguiFlipFrame();
        void ImguiUIManagement(const RenderContext& renderContext);
        void ImguiCullingTable(const RenderContext& renderContext);
        void ImguiGPUDebugMarkers(const RenderContext& renderContext);

        DescriptorHeap m_ImguiDescriptorHeap;
    };

} // namespace Boolka

#endif
