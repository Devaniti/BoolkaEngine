#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"
#include "PipelineState.h"

namespace Boolka
{
    class Device;
    class RootSignature;
    class InputLayout;

    class GraphicPipelineState : public PipelineState
    {
    public:
        GraphicPipelineState() = default;
        ~GraphicPipelineState() = default;

        bool Initialize(Device& device, RootSignature& rootSig, InputLayout& inputLayout,
                        const MemoryBlock& vertexShaderBytecode,
                        const MemoryBlock& pixelShaderBytecode, UINT renderTargetCount,
                        bool useDepthTest = false, bool writeDepth = true,
                        D3D12_COMPARISON_FUNC depthFunc = D3D12_COMPARISON_FUNC_LESS,
                        bool useAlphaBlend = false,
                        DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

        bool Initialize(Device& device, RootSignature& rootSig,
                        const MemoryBlock& amplificationShaderBytecode,
                        const MemoryBlock& meshShaderBytecode,
                        const MemoryBlock& pixelShaderBytecode, UINT renderTargetCount,
                        bool useDepthTest = false, bool writeDepth = true,
                        D3D12_COMPARISON_FUNC depthFunc = D3D12_COMPARISON_FUNC_LESS,
                        bool useAlphaBlend = false,
                        DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

    private:
        static void SetDefaultRasterizerDesc(D3D12_RASTERIZER_DESC& desc);
        static void SetDepthStencilDesc(D3D12_DEPTH_STENCIL_DESC& desc, bool useDepthTest,
                                        bool writeDepth, D3D12_COMPARISON_FUNC depthFunc);
        static void SetBlendDesc(D3D12_BLEND_DESC& blend, bool useAlphaBlend);
        static void SetRenderTargetFormats(D3D12_RT_FORMAT_ARRAY& renderTargetFormats,
                                           UINT renderTargetCount, DXGI_FORMAT renderTargetFormat);
    };

    BLK_IS_PLAIN_DATA_ASSERT(D3D12_GRAPHICS_PIPELINE_STATE_DESC);

} // namespace Boolka
