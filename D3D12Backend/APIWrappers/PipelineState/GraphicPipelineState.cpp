#include "stdafx.h"

#include "GraphicPipelineState.h"

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

} // namespace Boolka