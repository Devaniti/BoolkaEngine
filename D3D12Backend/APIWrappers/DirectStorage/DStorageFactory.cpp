#include "stdafx.h"

#include "DStorageFactory.h"

#include "Containers/Streaming/SceneData.h"

namespace Boolka
{

    DStorageFactory::DStorageFactory()
        : m_Factory(nullptr)
    {
    }

    DStorageFactory::~DStorageFactory()
    {
        BLK_ASSERT(m_Factory == nullptr);
    }

    IDStorageFactory* DStorageFactory::Get()
    {
        BLK_ASSERT(m_Factory != nullptr);
        return m_Factory;
    }

    IDStorageFactory* DStorageFactory::operator->()
    {
        return Get();
    }

    bool DStorageFactory::Initialize()
    {
        BLK_ASSERT(m_Factory == nullptr);
        HRESULT hr = DStorageGetFactory(IID_PPV_ARGS(&m_Factory));
        BLK_ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return false;

#ifdef BLK_DEBUG
            // m_Factory->SetDebugFlags(DSTORAGE_DEBUG_SHOW_ERRORS | DSTORAGE_DEBUG_BREAK_ON_ERROR |
            //                          DSTORAGE_DEBUG_RECORD_OBJECT_NAMES);
#endif

        m_Factory->SetStagingBufferSize(BLK_SCENE_MAX_ALLOWED_BUFFER_SIZE);
        return true;
    }

    void DStorageFactory::Unload()
    {
        BLK_ASSERT(m_Factory != nullptr);
        m_Factory->Release();
        m_Factory = nullptr;
    }

} // namespace Boolka