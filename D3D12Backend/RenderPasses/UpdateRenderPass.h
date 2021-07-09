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
        void UploadFrameConstantBuffer(RenderContext& renderContext,
                                       ResourceTracker& resourceTracker);
        void UploadLightingConstantBuffer(RenderContext& renderContext,
                                          ResourceTracker& resourceTracker);
        void UploadCullingConstantBuffer(RenderContext& renderContext,
                                         ResourceTracker& resourceTracker);
        void ReadbackDebugMarkersBuffer(RenderContext& renderContext,
                                        ResourceTracker& resourceTracker);

        ReadbackBuffer m_ReadbackBuffers[BLK_IN_FLIGHT_FRAMES];
    };

} // namespace Boolka
