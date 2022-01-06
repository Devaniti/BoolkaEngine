#pragma once
#include "Matrix.h"
#include "Vector.h"

namespace Boolka
{

    class AABB;

    class [[nodiscard]] Frustum
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

        [[nodiscard]] bool CheckPoint(const Vector4& point) const;
        [[nodiscard]] TestResult CheckSphere(const Vector4& center, float radius) const;
        [[nodiscard]] TestResult CheckAABB(const AABB& boundingBox) const;
        [[nodiscard]] TestResult CheckFrustum(const Matrix4x4& invViewMatrix,
                                              const Matrix4x4 invProjMatrix) const;

        // Fast variants return false if tested geometry is completely outside frustum, and true
        // otherwise
        // Faster since we don't need to distinguish between fully inside and intersection cases

        [[nodiscard]] bool CheckAABBFast(const AABB& boundingBox) const;
        [[nodiscard]] bool CheckSphereFast(const Vector4& center, float radius) const;
        [[nodiscard]] bool CheckFrustumFast(const Matrix4x4& invViewMatrix,
                                            const Matrix4x4 invProjMatrix) const;

        [[nodiscard]] float* GetBuffer();
        [[nodiscard]] const float* GetBuffer() const;

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
