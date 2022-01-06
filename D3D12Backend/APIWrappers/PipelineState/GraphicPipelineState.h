#pragma once
#include "APIWrappers/PipelineState/PipelineStateParameters.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"
#include "PipelineState.h"

namespace Boolka
{
    class Device;
    class RootSignature;
    class InputLayout;

    class [[nodiscard]] GraphicPipelineState : public PipelineState
    {
    public:
        GraphicPipelineState() = default;
        ~GraphicPipelineState() = default;

        template <typename... Args>
        bool Initialize(Device& device, const wchar_t* name, const Args&... args)
        {
            // Add default parameters in case they are missing in args list
            // TBD if we should error out instead
            if constexpr (!has_type<BlendParam, Args...>::value)
            {
                return Initialize(device, name, args..., BlendParam{});
            }

            if constexpr (!has_type<RasterizerParam, Args...>::value)
            {
                return Initialize(device, name, args..., RasterizerParam{});
            }

            if constexpr (!has_type<DepthStencilParam, Args...>::value)
            {
                return Initialize(device, name, args..., DepthStencilParam{});
            }

            if constexpr (!has_type<RenderTargetParam, Args...>::value)
            {
                return Initialize(device, name, args..., RenderTargetParam{});
            }

            // Construct PipelineStateStream from our parameter wrappers
            PipelineStateStream stateStream(args...);

            D3D12_PIPELINE_STATE_STREAM_DESC psoStreamDesc = {sizeof(stateStream), &stateStream};
            return InitializeInternal(device, name, psoStreamDesc);
        }

    private:
        bool InitializeInternal(Device& device, const wchar_t* name,
                                const D3D12_PIPELINE_STATE_STREAM_DESC& streamDesc);
    };

} // namespace Boolka
