#pragma once

#include "APIWrappers/PipelineState/ComputePipelineState.h"
#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "APIWrappers/PipelineState/StateObject.h"
#include "ShaderTable.h"

namespace Boolka
{

    class ComputePipelineState;

    class [[nodiscard]] PSOContainer
    {
    public:
        enum class GraphicPSO
        {
            DeferredLighting,
            GBuffer,
            ShadowMap,
            SkyBox,
            ToneMapping,
            ZBuffer,
            Count
        };

        enum class ComputePSO
        {
            ObjectCulling,
            CullingCommandBufferGeneration,
#ifdef BLK_ENABLE_STATS
            CullingDebugReadback,
#endif
            Count
        };

        enum class RTPSO
        {
            RaytracePass,
            Count
        };

        enum class RTShaderTable
        {
            Default,
            Count
        };

        PSOContainer() = default;
        ~PSOContainer() = default;

        bool Initialize(Device& device, RenderEngineContext& engineContext);
        void Unload();

        void FinishInitialization();

        [[nodiscard]] GraphicPipelineState& GetPSO(GraphicPSO id);
        [[nodiscard]] ComputePipelineState& GetPSO(ComputePSO id);
        [[nodiscard]] StateObject& GetPSO(RTPSO id);
        [[nodiscard]] ShaderTable& GetShaderTable(RTShaderTable id);

    private:
        GraphicPipelineState m_GraphicPSOs[static_cast<size_t>(GraphicPSO::Count)];
        ComputePipelineState m_ComputePSOs[static_cast<size_t>(ComputePSO::Count)];
        StateObject m_RTPSOs[static_cast<size_t>(RTPSO::Count)];
        ShaderTable m_RTShaderTables[static_cast<size_t>(RTShaderTable::Count)];
        UploadBuffer m_ShaderTablesUploadBuffer;
    };

} // namespace Boolka
