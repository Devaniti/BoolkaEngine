#include "stdafx.h"
#include "AABB.h"

namespace Boolka
{

    AABB::AABB(Vector3 min, Vector3 max)
        : m_min(min)
        , m_max(max)
    {
    }

}
