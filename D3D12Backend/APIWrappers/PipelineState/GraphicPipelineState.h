#pragma once
#include "APIWrappers/PipelineState/PipelineStateParameters.h"
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

        template <typename... Args>
        bool Initialize(Device& device, const Args&... args)
        {
            // Add default parameters in case they are missing in args list
            // TBD if we should error out instead
            if constexpr (!has_type<BlendParam, Args...>::value)
            {
                return Initialize(device, args..., BlendParam{});
            }

            if constexpr (!has_type<RasterizerParam, Args...>::value)
            {
                return Initialize(device, args..., RasterizerParam{});
            }

            if constexpr (!has_type<DepthStencilParam, Args...>::value)
            {
                return Initialize(device, args..., DepthStencilParam{});
            }

            if constexpr (!has_type<RenderTargetParam, Args...>::value)
            {
                return Initialize(device, args..., RenderTargetParam{});
            }

            // Construct PipelineStateStream from our parameter wrappers
            PipelineStateStream stateStream(args...);

            D3D12_PIPELINE_STATE_STREAM_DESC psoStreamDesc = {sizeof(stateStream), &stateStream};
            return InitializeInternal(device, psoStreamDesc);
        }

    private:
        bool InitializeInternal(Device& device, const D3D12_PIPELINE_STATE_STREAM_DESC& streamDesc);

        static void SetRasterizerDesc(D3D12_RASTERIZER_DESC& desc, float depthBias,
                                      float depthSlopeBias);
        static void SetDepthStencilDesc(D3D12_DEPTH_STENCIL_DESC& desc, bool useDepthTest,
                                        bool writeDepth, D3D12_COMPARISON_FUNC depthFunc);
        static void SetBlendDesc(D3D12_BLEND_DESC& blend, bool useAlphaBlend);
        static void SetRenderTargetFormats(D3D12_RT_FORMAT_ARRAY& renderTargetFormats,
                                           UINT renderTargetCount, DXGI_FORMAT renderTargetFormat);

        static INT ConvertFloatDepthBiasToDXDepthBias(float depthBias,
                                                      size_t formatMantissaBits = FLT_MANT_DIG);
    };

    BLK_IS_PLAIN_DATA_ASSERT(D3D12_GRAPHICS_PIPELINE_STATE_DESC);

} // namespace Boolka
