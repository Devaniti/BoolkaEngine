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

#ifdef BLK_ENABLE_PIPELINE_LIBRARY
        PipelineStateLibrary& PSOCache = device.GetPSOLibrary();

        BLK_RENDER_DEBUG_ONLY(device.FilterMessage(D3D12_MESSAGE_ID_LOADPIPELINE_NAMENOTFOUND));
        hr = PSOCache->LoadPipeline(name, &streamDesc, IID_PPV_ARGS(&state));
        BLK_RENDER_DEBUG_ONLY(device.RemoveLastMessageFilter());
        BLK_ASSERT(hr == E_INVALIDARG || SUCCEEDED(hr));
        if (SUCCEEDED(hr))
        {
            RenderDebug::SetDebugName(state, L"%ls", name);
            PipelineState::Initialize(state);
            return true;
        }
#endif

        hr = device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&state));
        BLK_ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return false;

        RenderDebug::SetDebugName(state, L"%ls", name);
        PipelineState::Initialize(state);

#ifdef BLK_ENABLE_PIPELINE_LIBRARY
        PSOCache.Store(device, name, *this);
#endif

        return true;
    }

} // namespace Boolka