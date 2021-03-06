#include "stdafx.h"

#include "Factory.h"

namespace Boolka
{

    Factory::Factory()
        : m_Factory(nullptr)
    {
    }

    Factory::~Factory()
    {
        BLK_ASSERT(m_Factory == nullptr);
    }

    IDXGIFactory7* Factory::Get()
    {
        BLK_ASSERT(m_Factory != nullptr);
        return m_Factory;
    }

    IDXGIFactory7* Factory::operator->()
    {
        return Get();
    }

    bool Factory::Initialize()
    {
        BLK_ASSERT(m_Factory == nullptr);

        UINT flags = 0;
#ifdef BLK_RENDER_DEBUG
        flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
        HRESULT hr = ::CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_Factory));
        return SUCCEEDED(hr);
    }

    void Factory::Unload()
    {
        BLK_ASSERT(m_Factory != nullptr);

        m_Factory->Release();
        m_Factory = nullptr;
    }

} // namespace Boolka