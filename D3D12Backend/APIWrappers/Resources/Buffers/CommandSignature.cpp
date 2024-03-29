#include "stdafx.h"

#include "CommandSignature.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    CommandSignature::CommandSignature()
        : m_CommandSignature(nullptr)
    {
    }

    CommandSignature::~CommandSignature()
    {
        BLK_ASSERT(m_CommandSignature == nullptr);
    }

    ID3D12CommandSignature* CommandSignature::Get()
    {
        BLK_ASSERT(m_CommandSignature != nullptr);
        return m_CommandSignature;
    }

    ID3D12CommandSignature* CommandSignature::operator->()
    {
        return Get();
    }

    bool CommandSignature::Initialize(Device& device, ID3D12RootSignature* rootSig,
                                      UINT commandStride, UINT argumentCount,
                                      const D3D12_INDIRECT_ARGUMENT_DESC* arguments)
    {
        BLK_ASSERT(m_CommandSignature == nullptr);

        D3D12_COMMAND_SIGNATURE_DESC desc = {};
        desc.ByteStride = commandStride;
        desc.NumArgumentDescs = argumentCount;
        desc.pArgumentDescs = arguments;

        HRESULT hr =
            device->CreateCommandSignature(&desc, rootSig, IID_PPV_ARGS(&m_CommandSignature));

        return SUCCEEDED(hr);
    }

    void CommandSignature::Unload()
    {
        BLK_ASSERT(m_CommandSignature != nullptr);
        m_CommandSignature->Release();
        m_CommandSignature = nullptr;
    }

} // namespace Boolka