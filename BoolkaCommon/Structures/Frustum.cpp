#include "stdafx.h"

#include "Frustum.h"

#include "AABB.h"

namespace Boolka
{

    Frustum::Frustum(const Matrix4x4& viewProj)
    {
        Matrix4x4 viewProjTransp = viewProj.Transpose();

        m_planes[0] = (viewProjTransp[3] - viewProjTransp[2]).Normalize3();
        m_planes[1] = (viewProjTransp[3] + viewProjTransp[2]).Normalize3();
        m_planes[2] = (viewProjTransp[3] - viewProjTransp[0]).Normalize3();
        m_planes[3] = (viewProjTransp[3] + viewProjTransp[0]).Normalize3();
        m_planes[4] = (viewProjTransp[3] - viewProjTransp[1]).Normalize3();
        m_planes[5] = (viewProjTransp[3] + viewProjTransp[1]).Normalize3();
    }

    bool Frustum::CheckPoint(const Vector4& point) const
    {
        BLK_ASSERT(point.w() == 1.0f);

        for (const auto& plane : m_planes)
        {
            if (plane.Dot(point) < 0.0f)
            {
                return false;
            }
        }

        return true;
    }

    Frustum::TestResult Frustum::CheckSphere(const Vector4& center, float radius) const
    {
        BLK_ASSERT(center.w() == 1.0f);

        TestResult result = Inside;

        for (const auto& plane : m_planes)
        {
            if (plane.Dot(center) < -radius)
            {
                return TestResult::Outside;
            }

            if (plane.Dot(center) < radius)
            {
                result = TestResult::Intersects;
            }
        }

        return result;
    }

    bool Frustum::CheckSphereFast(const Vector4& center, float radius) const
    {
        BLK_ASSERT(center.w() == 1.0f);

        // TODO rewrite, algorithm is incorrect
        for (const auto& plane : m_planes)
        {
            if (plane.Dot(center) < -radius)
            {
                return false;
            }
        }

        return true;
    }

    // Can potentially give false positive, in case when tested frustum is close to the corner of
    // this frustum
    // If checked both ways intersection test would be correct
    bool Frustum::CheckFrustumFast(const Matrix4x4& invViewMatrix,
                                   const Matrix4x4 invProjMatrix) const
    {
        static const Vector4 cornerPoints[8] = {{-1, -1, -1, 1}, {-1, -1, 1, 1}, {-1, 1, -1, 1},
                                                {-1, 1, 1, 1},   {1, -1, -1, 1}, {1, -1, 1, 1},
                                                {1, 1, -1, 1},   {1, 1, 1, 1}};

        Vector4 otherFrustumPoints[8];
        for (size_t i = 0; i < 8; ++i)
            otherFrustumPoints[i] = cornerPoints[i] * invProjMatrix;
        for (size_t i = 0; i < 8; ++i)
            otherFrustumPoints[i] = otherFrustumPoints[i] / otherFrustumPoints[i].w();
        for (size_t i = 0; i < 8; ++i)
            otherFrustumPoints[i] = otherFrustumPoints[i] * invViewMatrix;

        // If there's a plane, for which all points of another frustum is outside, then whole other
        // frustum is outside
        // If all points are on the correct side of all planes, then other frustum is fully inside

        for (const auto& plane : m_planes)
        {
            bool atLeastOneInside = false;
            for (size_t i = 0; i < 8; ++i)
            {
                float distanceToPlane = plane.Dot(otherFrustumPoints[i]);
                bool inside = distanceToPlane > 0.0f;
                if (inside)
                {
                    atLeastOneInside = true;
                    break;
                }
            }

            if (!atLeastOneInside)
                return false;
        }

        return true;
    }

    float* Frustum::GetBuffer()
    {
        return m_planes[0].GetBuffer();
    }

    const float* Frustum::GetBuffer() const
    {
        return m_planes[0].GetBuffer();
    }

    // Can potentially give false positive, in case when tested AABB is close to the corner of
    // the frustum
    Frustum::TestResult Frustum::CheckAABB(const AABB& boundingBox) const
    {
        BLK_ASSERT(boundingBox.GetMin().w() == 1.0f);
        BLK_ASSERT(boundingBox.GetMax().w() == 1.0f);

        TestResult result = Inside;
        Vector4 zero;
        const Vector4& aabbMin = boundingBox.GetMin();
        const Vector4& aabbMax = boundingBox.GetMax();

        // Batch by 2 planes
        for (int i = 0; i < 3; ++i)
        {
            Vector4 mask[2] = {
                m_planes[i * 2 + 0] > zero,
                m_planes[i * 2 + 1] > zero,
            };

            Vector4 max[2] = {
                aabbMax.Select(aabbMin, mask[0]),
                aabbMax.Select(aabbMin, mask[1]),
            };

            Vector4 min[2] = {
                aabbMin.Select(aabbMax, mask[0]),
                aabbMin.Select(aabbMax, mask[1]),
            };

            float dotMin[2] = {
                m_planes[i * 2 + 0].Dot(min[0]),
                m_planes[i * 2 + 1].Dot(min[1]),
            };

            float dotMax[2] = {
                m_planes[i * 2 + 0].Dot(max[0]),
                m_planes[i * 2 + 1].Dot(max[1]),
            };

            bool isOutside[2] = {
                dotMin[0] < 0.0f,
                dotMin[1] < 0.0f,
            };

            bool isIntersecs[2] = {
                dotMax[0] < 0.0f,
                dotMax[1] < 0.0f,
            };

            if (isOutside[0] || isOutside[1])
                return Outside;

            if (isIntersecs[0] || isIntersecs[1])
                result = Intersects;
        };

        return result;
    }

    // Can potentially give false positive, in case when tested frustum is close to the corner of
    // this frustum
    // If checked both ways intersection test would be correct
    Frustum::TestResult Frustum::CheckFrustum(const Matrix4x4& invViewMatrix,
                                              const Matrix4x4 invProjMatrix) const
    {
        static const Vector4 cornerPoints[8] = {{-1, -1, -1, 1}, {-1, -1, 1, 1}, {-1, 1, -1, 1},
                                                {-1, 1, 1, 1},   {1, -1, -1, 1}, {1, -1, 1, 1},
                                                {1, 1, -1, 1},   {1, 1, 1, 1}};

        Vector4 otherFrustumPoints[8];
        for (size_t i = 0; i < 8; ++i)
            otherFrustumPoints[i] = cornerPoints[i] * invProjMatrix;
        for (size_t i = 0; i < 8; ++i)
            otherFrustumPoints[i] = otherFrustumPoints[i] / otherFrustumPoints[i].w();
        for (size_t i = 0; i < 8; ++i)
            otherFrustumPoints[i] = otherFrustumPoints[i] * invViewMatrix;

        TestResult result = Inside;

        // If there's a plane, for which all points of another frustum is outside, then whole other
        // frustum is outside
        // If all points are on the correct side of all planes, then other frustum is fully inside

        for (const auto& plane : m_planes)
        {
            bool testResults[2] = {};
            for (size_t i = 0; i < 8; ++i)
            {
                float distanceToPlane = plane.Dot(otherFrustumPoints[i]);
                bool inside = distanceToPlane > 0.0f;
                testResults[inside] = true;
            }

            if (!testResults[true])
                return Outside;

            if (testResults[false])
                result = Intersects;
        }

        return result;
    }

    // Can potentially give false positive, in case when tested AABB is close to the corner of
    // the frustum
    bool Frustum::CheckAABBFast(const AABB& boundingBox) const
    {
        BLK_ASSERT(boundingBox.GetMin().w() == 1.0f);
        BLK_ASSERT(boundingBox.GetMax().w() == 1.0f);

        Vector4 zero;
        const Vector4& aabbMin = boundingBox.GetMin();
        const Vector4& aabbMax = boundingBox.GetMax();

        // Batch by 2 planes
        for (int i = 0; i < 3; ++i)
        {
            Vector4 mask[2] = {
                m_planes[i * 2 + 0] > zero,
                m_planes[i * 2 + 1] > zero,
            };

            Vector4 min[2] = {
                aabbMin.Select(aabbMax, mask[0]),
                aabbMin.Select(aabbMax, mask[1]),
            };

            float dotMin[2] = {
                m_planes[i * 2 + 0].Dot(min[0]),
                m_planes[i * 2 + 1].Dot(min[1]),
            };

            bool isOutside[2] = {
                dotMin[0] < 0.0f,
                dotMin[1] < 0.0f,
            };

            if (isOutside[0] || isOutside[1])
                return false;
        };

        return true;
    }

} // namespace Boolka
