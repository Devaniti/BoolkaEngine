#include "stdafx.h"

#include "StateObject.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    StateObject::StateObject()
        : m_StateObject(nullptr)
    {
    }

    StateObject::~StateObject()
    {
        BLK_ASSERT(m_StateObject == nullptr);
    }

    ID3D12StateObject* StateObject::Get()
    {
        BLK_ASSERT(m_StateObject != nullptr);
        return m_StateObject;
    }

    ID3D12StateObject* StateObject::operator->()
    {
        return Get();
    }

    void StateObject::Unload()
    {
        BLK_ASSERT(m_StateObject != nullptr);

        m_StateObject->Release();
        m_StateObject = nullptr;
    }

    bool StateObject::InitializeInternal(Device& device, const D3D12_STATE_OBJECT_DESC& desc)
    {
        HRESULT hr = device->CreateStateObject(&desc, IID_PPV_ARGS(&m_StateObject));
        BLK_ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return false;

        return true;
    }

} // namespace Boolka