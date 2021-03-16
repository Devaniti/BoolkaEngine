#pragma once
#include "Vector.h"

namespace Boolka
{

    class AABB
    {
    public:
        AABB() = default;
        ~AABB() = default;

        AABB(Vector4 min, Vector4 max);

        Vector4& GetMin();
        const Vector4& GetMin() const;
        Vector4& GetMax();
        const Vector4& GetMax() const;

    private:
        Vector4 m_min;
        Vector4 m_max;
    };

} // namespace Boolka
