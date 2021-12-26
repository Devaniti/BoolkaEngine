#pragma once
#include "APIWrappers/PipelineState/StateObject.h"
#include "Containers/ShaderTable.h"
#include "RenderPass.h"

namespace Boolka
{

    // Used for Reflection and Refraction
    class RaytraceRenderPass : public RenderPass
    {
    public:
        RaytraceRenderPass() = default;
        ~RaytraceRenderPass() = default;

        bool Initialize(Device& device, RenderContext& renderContext) final;
        void Unload() final;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) final;
        bool PrepareRendering() final;

    private:
        StateObject m_PSO;
        ShaderTable m_ShaderTable;
        Buffer m_ShaderTableBuffer;
    };

} // namespace Boolka
