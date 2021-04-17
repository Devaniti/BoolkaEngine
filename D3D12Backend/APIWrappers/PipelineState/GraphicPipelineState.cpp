#include "stdafx.h"

#include "GraphicPipelineState.h"

#include <Util/d3dx12.h>

#include "APIWrappers/Device.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    bool GraphicPipelineState::InitializeInternal(
        Device& device, const D3D12_PIPELINE_STATE_STREAM_DESC& streamDesc)
    {
        ID3D12PipelineState* state = nullptr;

        HRESULT hr = device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&state));
        if (FAILED(hr))
            return false;

        return PipelineState::Initialize(state);
    }

    void GraphicPipelineState::SetRasterizerDesc(D3D12_RASTERIZER_DESC& desc, float depthBias,
                                                 float depthSlopeBias)
    {
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = D3D12_CULL_MODE_BACK; // TODO handle special case for transparent
        desc.FrontCounterClockwise = TRUE;
        desc.DepthBias = ConvertFloatDepthBiasToDXDepthBias(depthBias);
        desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        desc.SlopeScaledDepthBias = depthSlopeBias;
        desc.DepthClipEnable = TRUE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        desc.ForcedSampleCount = 0;
        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    }

    void GraphicPipelineState::SetDepthStencilDesc(D3D12_DEPTH_STENCIL_DESC& desc,
                                                   bool useDepthTest, bool writeDepth,
                                                   D3D12_COMPARISON_FUNC depthFunc)
    {
        desc.DepthEnable = FALSE;
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        desc.StencilEnable = FALSE;
        desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        desc.BackFace = desc.FrontFace;

        if (useDepthTest)
        {
            desc.DepthEnable = true;
            desc.DepthFunc = depthFunc;
            desc.DepthWriteMask =
                writeDepth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        }
        else
        {
            desc.DepthEnable = false;
        }
    }

    void GraphicPipelineState::SetBlendDesc(D3D12_BLEND_DESC& blend, bool useAlphaBlend)
    {
        blend.AlphaToCoverageEnable = FALSE;
        blend.IndependentBlendEnable = FALSE;

        auto& renderTarget = blend.RenderTarget[0];
        renderTarget.LogicOp = D3D12_LOGIC_OP_CLEAR;
        renderTarget.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_RED |
                                             D3D12_COLOR_WRITE_ENABLE_GREEN |
                                             D3D12_COLOR_WRITE_ENABLE_BLUE;
        if (useAlphaBlend)
        {
            renderTarget.BlendEnable = TRUE;
            renderTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            renderTarget.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            renderTarget.BlendOp = D3D12_BLEND_OP_ADD;
        }
        else
        {
            renderTarget.BlendEnable = FALSE;
            renderTarget.SrcBlend = D3D12_BLEND_ONE;
            renderTarget.DestBlend = D3D12_BLEND_ZERO;
            renderTarget.BlendOp = D3D12_BLEND_OP_ADD;
        }
        renderTarget.SrcBlendAlpha = D3D12_BLEND_ZERO;
        renderTarget.DestBlendAlpha = D3D12_BLEND_ZERO;
        renderTarget.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    }

    void GraphicPipelineState::SetRenderTargetFormats(D3D12_RT_FORMAT_ARRAY& renderTargetFormats,
                                                      UINT renderTargetCount,
                                                      DXGI_FORMAT renderTargetFormat)
    {
        renderTargetFormats.NumRenderTargets = renderTargetCount;
        for (UINT i = 0; i < renderTargetCount; ++i)
        {
            renderTargetFormats.RTFormats[i] = renderTargetFormat;
        }
    }

    INT GraphicPipelineState::ConvertFloatDepthBiasToDXDepthBias(
        float depthBias, size_t formatMantissaBits /*= FLT_MANT_DIG*/)
    {
        // Minus one because DX doesn't count hidden bit
        size_t mantissaDigits = formatMantissaBits - 1;
        return static_cast<INT>(depthBias * (::powf(2, static_cast<float>(mantissaDigits))));
    }

} // namespace Boolka