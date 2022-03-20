#include "stdafx.h"

#include "GraphicPipelineState.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    bool GraphicPipelineState::InitializeInternal(
        Device& device, const wchar_t* name, const D3D12_PIPELINE_STATE_STREAM_DESC& streamDesc)
    {
        ID3D12PipelineState* state = nullptr;
        HRESULT hr;

        hr = device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&state));
        BLK_ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return false;

        RenderDebug::SetDebugName(state, L"%ls", name);
        PipelineState::Initialize(state);

        return true;
    }

} // namespace Boolka