#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "Contexts/FrameStats.h"
#include "DebugHelpers/ImguiGraphHelper.h"
#include "RenderPass.h"

#ifdef BLK_ENABLE_STATS

namespace Boolka
{

    class DebugOverlayPass : public RenderPass
    {
    public:
        DebugOverlayPass();
        ~DebugOverlayPass();

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
        void ImguiFlipFrame();
        void ImguiUIManagement(const RenderContext& renderContext);
        void ImguiCullingTable(const RenderContext& renderContext);
        void ImguiGPUDebugMarkers(const RenderContext& renderContext);
        void ImguiUIGPUTimes(const RenderContext& renderContext);
        void ImguiGraphs(const RenderContext& renderContext);

        DescriptorHeap m_ImguiDescriptorHeap;

        float m_ScaleFactor;

        ImguiGraphHelper m_FPSGraph;
        ImguiGraphHelper m_FrameTimeGraph;
        ImguiGraphHelper m_GPUTime;

        ImguiGraphHelper m_GPUPassGraphs[ARRAYSIZE(FrameStats::GPUTimes::Markers)];
    };

} // namespace Boolka

#endif
