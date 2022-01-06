#pragma once
#include "Vector.h"

namespace Boolka
{

    class AABB;

    class [[nodiscard]] Sphere
    {
    public:
        Sphere() = default;
        ~Sphere() = default;

        Sphere(const Vector3& center, float radiusSqr);
        Sphere(const Vector4& sphere);

        [[nodiscard]] static Sphere BuildBoundingSphere(const Vector4* verticies,
                                                        size_t vertexCount);

        [[nodiscard]] const Vector4& GetData();

    private:
        // xyz - center, w - radius squared
        Vector4 m_Sphere;
    };

} // namespace Boolka
