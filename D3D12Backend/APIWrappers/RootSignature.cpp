#include "stdafx.h"
#include "RootSignature.h"

#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"

#include "APIWrappers/Device.h"

namespace Boolka
{


    RootSignature::RootSignature()
        : m_RootSignature(nullptr)
    {
    }

    RootSignature::~RootSignature()
    {
        BLK_ASSERT(m_RootSignature == nullptr);
    }

    bool RootSignature::Initialize(Device& device, const char* filename)
    {
        BLK_ASSERT(m_RootSignature == nullptr);

        MemoryBlock compiledRootSignature = DebugFileReader::ReadFile(filename);
        HRESULT hr = device->CreateRootSignature(0, compiledRootSignature.m_Data, compiledRootSignature.m_Size, IID_PPV_ARGS(&m_RootSignature));
        DebugFileReader::FreeMemory(compiledRootSignature);
        return SUCCEEDED(hr);
    }

    void RootSignature::Unload()
    {
        BLK_ASSERT(m_RootSignature != nullptr);
        m_RootSignature->Release();
        m_RootSignature = nullptr;
    }

}