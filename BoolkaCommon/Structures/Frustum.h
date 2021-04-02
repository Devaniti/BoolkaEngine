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

        Frustum(const Matrix4x4& viewProj);

        bool CheckPoint(const Vector4& point) const;
        TestResult CheckSphere(const Vector4& center, float radius) const;
        TestResult CheckAABB(const AABB& boundingBox) const;
        TestResult CheckFrustum(const Matrix4x4& invViewMatrix,
                                const Matrix4x4 invProjMatrix) const;

        // Fast variants return false if tested geometry is completely outside frustum, and true
        // otherwise
        // Faster since we don't need to distinguish between fully inside and intersection cases

        bool CheckAABBFast(const AABB& boundingBox) const;
        bool CheckSphereFast(const Vector4& center, float radius) const;
        bool CheckFrustumFast(const Matrix4x4& invViewMatrix, const Matrix4x4 invProjMatrix) const;

        float* GetBuffer();
        const float* GetBuffer() const;

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

} // namespace Boolka
