#include "stdafx.h"
#include "GraphicPipelineState.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/RootSignature.h"
#include "APIWrappers/InputLayout.h"

namespace Boolka
{

    bool GraphicPipelineState::Initialize(Device& device,
        RootSignature& rootSig,
        InputLayout& inputLayout,
        const MemoryBlock& vertexShaderBytecode,
        const MemoryBlock& pixelShaderBytecode,
        bool useDepthTest /*= false*/)
    {
        ID3D12PipelineState* state = nullptr;
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;

        SetDefaultPipelineStateDesc(desc);

        if (useDepthTest)
        {
            desc.DepthStencilState.DepthEnable = TRUE;
            desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        }

        desc.pRootSignature = rootSig.Get();
        desc.VS = D3D12_SHADER_BYTECODE{ vertexShaderBytecode.m_Data, vertexShaderBytecode.m_Size };
        desc.PS = D3D12_SHADER_BYTECODE{ pixelShaderBytecode.m_Data, pixelShaderBytecode.m_Size };
        inputLayout.FillInputLayoutDesc(desc.InputLayout);

        desc.SampleMask = UINT32_MAX;
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
        desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
        desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        desc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
        desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&state));
        if (FAILED(hr)) return false;

        return PipelineState::Initialize(state);
    }

    void GraphicPipelineState::SetDefaultPipelineStateDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
    {
        desc = {};
        SetDefaultRasterizerDesc(desc.RasterizerState);
        SetDefaultDepthStencilDesc(desc.DepthStencilState);
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
    }

    void GraphicPipelineState::SetDefaultRasterizerDesc(D3D12_RASTERIZER_DESC& desc)
    {
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = D3D12_CULL_MODE_BACK;
        desc.FrontCounterClockwise = TRUE;
        desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        desc.DepthClipEnable = TRUE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        desc.ForcedSampleCount = 0;
        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    }

    void GraphicPipelineState::SetDefaultDepthStencilDesc(D3D12_DEPTH_STENCIL_DESC& desc)
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
    }

}