#pragma once

#include "APIWrappers/InputLayout.h"
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    template <typename argumentType>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor;

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<D3D12_PIPELINE_STATE_FLAGS>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(D3D12_PIPELINE_STATE_FLAGS flags)
            : Flags(flags){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS;
        D3D12_PIPELINE_STATE_FLAGS Flags;
    };

    struct NodeMaskPipelineStateParam
    {
        UINT NodeMask = 0;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<NodeMaskPipelineStateParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(NodeMaskPipelineStateParam nodeMaskParam)
            : NodeMask(nodeMaskParam.NodeMask){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK;
        UINT NodeMask;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<RootSignature>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const RootSignature& rootSig)
            : RootSig(rootSig.Get()){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type =
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
        ID3D12RootSignature* RootSig;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<InputLayout>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const InputLayout& inputLayout)
        {
            inputLayout.FillInputLayoutDesc(InputLayoutDesc);
        };

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT;
        D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
    };

    template <>
    struct alignas(void*)
        [[nodiscard]] PipelineStateArgumentConvertor<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ibStripCutValue)
            : IbStripCutValue(ibStripCutValue){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type =
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE;
        D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IbStripCutValue;
    };

    template <>
    struct alignas(void*)
        [[nodiscard]] PipelineStateArgumentConvertor<D3D12_PRIMITIVE_TOPOLOGY_TYPE>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
            : TopologyType(topologyType){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type =
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY;
        D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType;
    };

    struct ShaderParam
    {
        ShaderParam(MemoryBlock& memoryBlock)
            : bytecode{memoryBlock.m_Data, memoryBlock.m_Size} {};
        D3D12_SHADER_BYTECODE bytecode;
    };

    struct VSParam : public ShaderParam
    {
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<VSParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const VSParam& shaderBytecode)
            : ShaderBytecode(shaderBytecode.bytecode){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS;
        D3D12_SHADER_BYTECODE ShaderBytecode;
    };

    struct GSParam : public ShaderParam
    {
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<GSParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const GSParam& shaderBytecode)
            : ShaderBytecode(shaderBytecode.bytecode){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS;
        D3D12_SHADER_BYTECODE ShaderBytecode;
    };

    struct HSParam : public ShaderParam
    {
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<HSParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const HSParam& shaderBytecode)
            : ShaderBytecode(shaderBytecode.bytecode){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS;
        D3D12_SHADER_BYTECODE ShaderBytecode;
    };

    struct DSParam : public ShaderParam
    {
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<DSParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const DSParam& shaderBytecode)
            : ShaderBytecode(shaderBytecode.bytecode){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS;
        D3D12_SHADER_BYTECODE ShaderBytecode;
    };

    struct PSParam : public ShaderParam
    {
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<PSParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const PSParam& shaderBytecode)
            : ShaderBytecode(shaderBytecode.bytecode){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS;
        D3D12_SHADER_BYTECODE ShaderBytecode;
    };

    struct ASParam : public ShaderParam
    {
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<ASParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const ASParam& shaderBytecode)
            : ShaderBytecode(shaderBytecode.bytecode){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS;
        D3D12_SHADER_BYTECODE ShaderBytecode;
    };

    struct MSParam : public ShaderParam
    {
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<MSParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const MSParam& shaderBytecode)
            : ShaderBytecode(shaderBytecode.bytecode){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS;
        D3D12_SHADER_BYTECODE ShaderBytecode;
    };

    struct CSParam : public ShaderParam
    {
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<CSParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const CSParam& shaderBytecode)
            : ShaderBytecode(shaderBytecode.bytecode){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS;
        D3D12_SHADER_BYTECODE ShaderBytecode;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<D3D12_STREAM_OUTPUT_DESC>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const D3D12_STREAM_OUTPUT_DESC& streamOut)
            : StreamOut(streamOut){};

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type =
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT;
        D3D12_STREAM_OUTPUT_DESC StreamOut;
    };

    struct BlendParam
    {
        bool useAlphaBlend = false;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<BlendParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const BlendParam& blendDesc)
            : BlendDesc{}
        {
            BlendDesc.AlphaToCoverageEnable = FALSE;
            BlendDesc.IndependentBlendEnable = FALSE;

            auto& renderTarget = BlendDesc.RenderTarget[0];
            renderTarget.LogicOp = D3D12_LOGIC_OP_CLEAR;
            renderTarget.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
            if (blendDesc.useAlphaBlend)
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
        };

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND;
        D3D12_BLEND_DESC BlendDesc;
    };

    struct DepthStencilParam
    {
        bool UseDepthTest = true;
        bool WriteDepth = false;
        D3D12_COMPARISON_FUNC DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<DepthStencilParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const DepthStencilParam& params)
            : DepthStencilDesc{}
        {
            DepthStencilDesc.DepthEnable = FALSE;
            DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            DepthStencilDesc.StencilEnable = FALSE;
            DepthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
            DepthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
            DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
            DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
            DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
            DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;

            if (params.UseDepthTest)
            {
                DepthStencilDesc.DepthEnable = true;
                DepthStencilDesc.DepthFunc = params.DepthFunc;
                DepthStencilDesc.DepthWriteMask =
                    params.WriteDepth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
            }
            else
            {
                DepthStencilDesc.DepthEnable = false;
            }
        }

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type =
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL;
        D3D12_DEPTH_STENCIL_DESC DepthStencilDesc;
    };

    struct DepthFormatParam
    {
        DXGI_FORMAT DepthFormat = DXGI_FORMAT_D32_FLOAT;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<DepthFormatParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(DepthFormatParam depthFormatParam)
            : DepthFormat(depthFormatParam.DepthFormat)
        {
        }

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type =
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT;
        DXGI_FORMAT DepthFormat;
    };

    inline INT ConvertFloatDepthBiasToDXDepthBias(float depthBias,
                                                  size_t formatMantissaBits = FLT_MANT_DIG)
    {
        // Minus one because DX doesn't count hidden bit
        size_t mantissaDigits = formatMantissaBits - 1;
        return static_cast<INT>(depthBias * (::powf(2, static_cast<float>(mantissaDigits))));
    }

    struct RasterizerParam
    {
        float depthBias = 0.0f;
        float depthSlopeBias = 0.0f;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<RasterizerParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const RasterizerParam& rasterizerParam)
            : RasterizerDesc{}
        {
            RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
            RasterizerDesc.CullMode =
                D3D12_CULL_MODE_BACK; // TODO handle special case for transparent
            RasterizerDesc.FrontCounterClockwise = TRUE;
            RasterizerDesc.DepthBias =
                ConvertFloatDepthBiasToDXDepthBias(rasterizerParam.depthBias);
            RasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
            RasterizerDesc.SlopeScaledDepthBias = rasterizerParam.depthSlopeBias;
            RasterizerDesc.DepthClipEnable = TRUE;
            RasterizerDesc.MultisampleEnable = FALSE;
            RasterizerDesc.AntialiasedLineEnable = FALSE;
            RasterizerDesc.ForcedSampleCount = 0;
            RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        }

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER;
        D3D12_RASTERIZER_DESC RasterizerDesc;
    };

    struct RenderTargetParam
    {
        UINT RenderTargetCount = 1;
        DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<RenderTargetParam>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const RenderTargetParam& renderTargetParam)
            : RenderTargetFormats{}
        {
            RenderTargetFormats.NumRenderTargets = renderTargetParam.RenderTargetCount;
            for (UINT i = 0; i < renderTargetParam.RenderTargetCount; ++i)
            {
                RenderTargetFormats.RTFormats[i] = renderTargetParam.RenderTargetFormat;
            }
        }

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type =
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS;
        D3D12_RT_FORMAT_ARRAY RenderTargetFormats;
    };

    template <>
    struct alignas(void*) [[nodiscard]] PipelineStateArgumentConvertor<DXGI_SAMPLE_DESC>
    {
        PipelineStateArgumentConvertor() = delete;
        PipelineStateArgumentConvertor(const DXGI_SAMPLE_DESC& sampleDesc)
            : SampleDesc(sampleDesc)
        {
        }

        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC;
        DXGI_SAMPLE_DESC SampleDesc;
    };

    // Converts list of wrappers to DX12 readable struct with correct memory layout
    template <typename FirstParam, typename... ArgsType>
    struct alignas(void*) [[nodiscard]] PipelineStateStream
    {
        PipelineStateStream() = delete;
        PipelineStateStream(const FirstParam& firstParam, const ArgsType&... args)
            : Data(firstParam, args...){};

        struct [[nodiscard]] Wrapper1
        {
            PipelineStateArgumentConvertor<FirstParam> FirstParamValue;
            PipelineStateStream<ArgsType...> Tail;

            Wrapper1(const FirstParam& firstParam, const ArgsType&... args)
                : FirstParamValue(firstParam)
                , Tail(args...){};
        };
        struct [[nodiscard]] Wrapper2
        {
            PipelineStateArgumentConvertor<FirstParam> FirstParamValue;

            Wrapper2(const FirstParam& firstParam)
                : FirstParamValue(firstParam){};
        };

        std::conditional_t<sizeof...(ArgsType) != 0, Wrapper1, Wrapper2> Data;
    };

} // namespace Boolka
