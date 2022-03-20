#include "stdafx.h"

#include "StateObject.h"

#include "APIWrappers/Device.h"
#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"

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

    bool StateObject::InitializeInternal(Device& device, const wchar_t* name,
                                         const D3D12_STATE_OBJECT_DESC& desc)
    {
        BLK_CPU_SCOPE("StateObject::InitializeInternal");

        HRESULT hr = device->CreateStateObject(&desc, IID_PPV_ARGS(&m_StateObject));
        BLK_ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return false;

        RenderDebug::SetDebugName(m_StateObject, L"%ls", name);

        return true;
    }

} // namespace Boolka