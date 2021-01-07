#pragma once
#include "PipelineState.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka {  }

namespace Boolka
{
    class Device;
    class RootSignature;
    class InputLayout;

    class GraphicPipelineState :
        public PipelineState
    {
    public:
        GraphicPipelineState() = default;
        ~GraphicPipelineState() = default;

        bool Initialize(Device& device, 
            RootSignature& rootSig,
            InputLayout& inputLayout,
            const MemoryBlock& vertexShaderBytecode, 
            const MemoryBlock& pixelShaderBytecode,
            UINT renderTargetCount,
            bool useDepthTest = false,
            bool writeDepth = true,
            D3D12_COMPARISON_FUNC depthFunc = D3D12_COMPARISON_FUNC_LESS);
    private:
        static void SetDefaultPipelineStateDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
        static void SetDefaultRasterizerDesc(D3D12_RASTERIZER_DESC& desc);
        static void SetDefaultDepthStencilDesc(D3D12_DEPTH_STENCIL_DESC& desc);
    };

    IS_PLAIN_DATA_ASSERT(D3D12_GRAPHICS_PIPELINE_STATE_DESC);

}
