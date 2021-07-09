#include "stdafx.h"

#include "Resource.h"

namespace Boolka
{

    ID3D12Resource* Resource::Get()
    {
        BLK_ASSERT(m_Resource != nullptr);
        return m_Resource;
    }

    ID3D12Resource* Resource::operator->()
    {
        return Get();
    }

    Resource::Resource()
        : m_Resource(nullptr)
    {
    }

    Resource::~Resource()
    {
        BLK_ASSERT(m_Resource == nullptr);
    }

} // namespace Boolka
