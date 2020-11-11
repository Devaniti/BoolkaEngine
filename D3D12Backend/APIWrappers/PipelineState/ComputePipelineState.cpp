#include "stdafx.h"
#include "ComputePipelineState.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    bool ComputePipelineState::Initialize(Device& device, RootSignature& rootSig, const MemoryBlock& computeShaderBytecode)
    {
        ID3D12PipelineState* state = nullptr;
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};

        desc.pRootSignature = rootSig.Get();
        desc.CS = D3D12_SHADER_BYTECODE{ computeShaderBytecode.m_Data, computeShaderBytecode.m_Size };
        
        HRESULT hr = device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&state));
        if (FAILED(hr)) return false;

        return PipelineState::Initialize(state);
    }

}
