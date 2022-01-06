#pragma once
#include "Vector.h"

namespace Boolka
{

    class [[nodiscard]] AABB
    {
    public:
        AABB() = default;
        ~AABB() = default;

        AABB(Vector4 min, Vector4 max);

        [[nodiscard]] Vector4& GetMin();
        [[nodiscard]] const Vector4& GetMin() const;
        [[nodiscard]] Vector4& GetMax();
        [[nodiscard]] const Vector4& GetMax() const;

    private:
        Vector4 m_min;
        Vector4 m_max;
    };

} // namespace Boolka
