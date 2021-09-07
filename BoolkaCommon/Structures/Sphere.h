#pragma once
#include "Vector.h"

namespace Boolka
{

    class AABB;

    class Sphere
    {
    public:
        Sphere() = default;
        ~Sphere() = default;

        Sphere(const Vector3& center, float radiusSqr);
        Sphere(const Vector4& sphere);

        static Sphere BuildBoundingSphere(const Vector4* verticies, size_t vertexCount);

        const Vector4& GetData();
    private:
        // xyz - center, w - radius squared
        Vector4 m_Sphere;
    };

} // namespace Boolka
