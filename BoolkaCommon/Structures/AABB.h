#pragma once
#include "Vector.h"

namespace Boolka
{

    class AABB
    {
    public:
        AABB() = default;
        ~AABB() = default;

        AABB(Vector3 min, Vector3 max);

        Vector3& GetMin() { return m_min; };
        const Vector3& GetMin() const { return m_min; };
        Vector3& GetMax() { return m_max; };
        const Vector3& GetMax() const { return m_max; };

    private:
        Vector3 m_min;
        Vector3 m_max;
    };

}
