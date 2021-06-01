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
    };

    BLK_IS_PLAIN_DATA_ASSERT(D3D12_GRAPHICS_PIPELINE_STATE_DESC);

} // namespace Boolka
