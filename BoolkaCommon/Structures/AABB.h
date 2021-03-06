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

        Vector3& GetMin();
        const Vector3& GetMin() const;
        Vector3& GetMax();
        const Vector3& GetMax() const;

    private:
        Vector3 m_min;
        Vector3 m_max;
    };

} // namespace Boolka
