#include "stdafx.h"

#include "GraphicPipelineState.h"

#include <Util/d3dx12.h>

#include "APIWrappers/Device.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    bool GraphicPipelineState::Initialize(
        Device& device, RootSignature& rootSig, InputLayout& inputLayout,
        const MemoryBlock& vertexShaderBytecode, const MemoryBlock& pixelShaderBytecode,
        UINT renderTargetCount, bool useDepthTest /*= false*/, bool writeDepth /*= true*/,
        D3D12_COMPARISON_FUNC depthFunc /*= D3D12_COMPARISON_FUNC_LESS*/,
        bool useAlphaBlend /*= false*/,
        DXGI_FORMAT renderTargetFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/)
    {
        struct alignas(void*) PipelineStateStream
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE rootSig;
            CD3DX12_PIPELINE_STATE_STREAM_VS vs;
            CD3DX12_PIPELINE_STATE_STREAM_PS ps;
            CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC blend;
            CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL depthStencil;
            CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT inputLayout;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY primitiveTopology;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS renderTargetFormats;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT depthStencilFormat;
        } psoStream{};

        D3D12_PIPELINE_STATE_STREAM_DESC psoStreamDesc = {sizeof(psoStream), &psoStream};

        psoStream.rootSig = rootSig.Get();
        psoStream.vs =
            D3D12_SHADER_BYTECODE{vertexShaderBytecode.m_Data, vertexShaderBytecode.m_Size};
        psoStream.ps =
            D3D12_SHADER_BYTECODE{pixelShaderBytecode.m_Data, pixelShaderBytecode.m_Size};
        SetBlendDesc(psoStream.blend, useAlphaBlend);
        SetDefaultRasterizerDesc(psoStream.rasterizer);
        SetDepthStencilDesc(psoStream.depthStencil, useDepthTest, writeDepth, depthFunc);
        inputLayout.FillInputLayoutDesc(psoStream.inputLayout);
        psoStream.primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        SetRenderTargetFormats(psoStream.renderTargetFormats, renderTargetCount,
                               renderTargetFormat);
        psoStream.depthStencilFormat = DXGI_FORMAT_D32_FLOAT;

        ID3D12PipelineState* state = nullptr;

        HRESULT hr = device->CreatePipelineState(&psoStreamDesc, IID_PPV_ARGS(&state));
        if (FAILED(hr))
            return false;

        return PipelineState::Initialize(state);
    }

    bool GraphicPipelineState::Initialize(
        Device& device, RootSignature& rootSig, const MemoryBlock& amplificationShaderBytecode,
        const MemoryBlock& meshShaderBytecode, const MemoryBlock& pixelShaderBytecode,
        UINT renderTargetCount, bool useDepthTest /*= false*/, bool writeDepth /*= true*/,
        D3D12_COMPARISON_FUNC depthFunc /*= D3D12_COMPARISON_FUNC_LESS*/,
        bool useAlphaBlend /*= false*/,
        DXGI_FORMAT renderTargetFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/)
    {
        struct PipelineStateStream
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE rootSig;
            CD3DX12_PIPELINE_STATE_STREAM_AS as;
            CD3DX12_PIPELINE_STATE_STREAM_MS ms;
            CD3DX12_PIPELINE_STATE_STREAM_PS ps;
            CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC blend;
            CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL depthStencil;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS renderTargetFormats;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT depthStencilFormat;
        } psoStream{};

        D3D12_PIPELINE_STATE_STREAM_DESC psoStreamDesc = {sizeof(psoStream), &psoStream};

        psoStream.rootSig = rootSig.Get();
        psoStream.as = D3D12_SHADER_BYTECODE{amplificationShaderBytecode.m_Data,
                                             amplificationShaderBytecode.m_Size};
        psoStream.ms = D3D12_SHADER_BYTECODE{meshShaderBytecode.m_Data, meshShaderBytecode.m_Size};
        psoStream.ps =
            D3D12_SHADER_BYTECODE{pixelShaderBytecode.m_Data, pixelShaderBytecode.m_Size};
        SetBlendDesc(psoStream.blend, useAlphaBlend);
        SetDefaultRasterizerDesc(psoStream.rasterizer);
        SetDepthStencilDesc(psoStream.depthStencil, useDepthTest, writeDepth, depthFunc);
        SetRenderTargetFormats(psoStream.renderTargetFormats, renderTargetCount,
                               renderTargetFormat);
        psoStream.depthStencilFormat = DXGI_FORMAT_D32_FLOAT;

        ID3D12PipelineState* state = nullptr;

        HRESULT hr = device->CreatePipelineState(&psoStreamDesc, IID_PPV_ARGS(&state));
        if (FAILED(hr))
            return false;

        return PipelineState::Initialize(state);
    }

    void GraphicPipelineState::SetDefaultRasterizerDesc(D3D12_RASTERIZER_DESC& desc)
    {
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = D3D12_CULL_MODE_BACK; // TODO handle special case for transparent
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

} // namespace Boolka