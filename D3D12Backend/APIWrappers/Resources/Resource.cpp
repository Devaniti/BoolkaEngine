#include "stdafx.h"
#include "Resource.h"

namespace Boolka
{

    Resource::Resource()
        : m_Resource(nullptr)
    {
    }

    Resource::~Resource()
    {
        BLK_ASSERT(m_Resource == nullptr);
    }

}
