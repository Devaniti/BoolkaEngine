#pragma once

namespace Boolka
{

    class AABB;

    class Frustum
    {
    public:

        enum TestResult
        {
            Outside = 0,
            Intersects = 1,
            Inside = 2,
        };

        Frustum() = default;
        ~Frustum() = default;

        Frustum(Matrix4x4 viewProj);

        bool CheckPoint(const Vector4& point);
        bool CheckSphere(const Vector4& center, float radius);
        TestResult CheckAABB(const AABB& boundingBox);

    private:
        // 6 Planes of frustum in next order
        // 0 - Near
        // 1 - Far
        // 2 - Left
        // 3 - Right
        // 4 - Top
        // 5 - Bottom
        Vector4 m_planes[6];
    };

}
