#include "stdafx.h"

#include "AABB.h"

namespace Boolka
{

    AABB::AABB(Vector4 min, Vector4 max)
        : m_min(min)
        , m_max(max)
    {
    }

    const Vector4& AABB::GetMin() const
    {
        return m_min;
    }

    Vector4& AABB::GetMin()
    {
        return m_min;
    }

    const Vector4& AABB::GetMax() const
    {
        return m_max;
    }

    Vector4& AABB::GetMax()
    {
        return m_max;
    }

} // namespace Boolka
