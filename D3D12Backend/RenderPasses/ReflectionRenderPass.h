#pragma once
#include "APIWrappers/PipelineState/StateObject.h"
#include "Containers/ShaderTable.h"
#include "RenderPass.h"

namespace Boolka
{

    class ReflectionRenderPass : public RenderPass
    {
    public:
        ReflectionRenderPass() = default;
        ~ReflectionRenderPass() = default;

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
