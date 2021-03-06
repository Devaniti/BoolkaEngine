#include "stdafx.h"

#include "AABB.h"

namespace Boolka
{

    AABB::AABB(Vector3 min, Vector3 max)
        : m_min(min)
        , m_max(max)
    {
    }

    const Vector3& AABB::GetMin() const
    {
        return m_min;
    }

    Vector3& AABB::GetMin()
    {
        return m_min;
    }

    const Vector3& AABB::GetMax() const
    {
        return m_max;
    }

    Vector3& AABB::GetMax()
    {
        return m_max;
    }

} // namespace Boolka
