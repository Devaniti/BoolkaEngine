#include "stdafx.h"

#include "ComputePipelineState.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    bool ComputePipelineState::Initialize(Device& device, const wchar_t* name,
                                          RootSignature& rootSig,
                                          const MemoryBlock& computeShaderBytecode)
    {
        ID3D12PipelineState* state = nullptr;
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};

        desc.pRootSignature = rootSig.Get();
        desc.CS = D3D12_SHADER_BYTECODE{computeShaderBytecode.m_Data, computeShaderBytecode.m_Size};

        HRESULT hr;
#ifdef BLK_ENABLE_PIPELINE_LIBRARY
        PipelineStateLibrary& PSOCache = device.GetPSOLibrary();

        BLK_RENDER_DEBUG_ONLY(device.FilterMessage(D3D12_MESSAGE_ID_LOADPIPELINE_NAMENOTFOUND));
        hr = PSOCache->LoadComputePipeline(name, &desc, IID_PPV_ARGS(&state));
        BLK_RENDER_DEBUG_ONLY(device.RemoveLastMessageFilter());
        BLK_ASSERT(hr == E_INVALIDARG || SUCCEEDED(hr));
        if (SUCCEEDED(hr))
        {
            RenderDebug::SetDebugName(state, L"%ls", name);
            PipelineState::Initialize(state);
            return true;
        }
#endif

        hr = device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&state));
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
