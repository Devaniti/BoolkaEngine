#include "stdafx.h"

#include "Frustum.h"

#include "AABB.h"

namespace Boolka
{

    Frustum::Frustum(Matrix4x4 viewProj)
    {
        // Near
        m_planes[0][0] = -viewProj[0][3] + viewProj[0][2];
        m_planes[0][1] = -viewProj[1][3] + viewProj[1][2];
        m_planes[0][2] = -viewProj[2][3] + viewProj[2][2];
        m_planes[0][3] = -viewProj[3][3] + viewProj[3][2];
        m_planes[0] = m_planes[0].Normalize3();

        // Far
        m_planes[1][0] = -viewProj[0][3] - viewProj[0][2];
        m_planes[1][1] = -viewProj[1][3] - viewProj[1][2];
        m_planes[1][2] = -viewProj[2][3] - viewProj[2][2];
        m_planes[1][3] = -viewProj[3][3] - viewProj[3][2];
        m_planes[1] = m_planes[1].Normalize3();

        // Left
        m_planes[2][0] = -viewProj[0][3] + viewProj[0][0];
        m_planes[2][1] = -viewProj[1][3] + viewProj[1][0];
        m_planes[2][2] = -viewProj[2][3] + viewProj[2][0];
        m_planes[2][3] = -viewProj[3][3] + viewProj[3][0];
        m_planes[2] = m_planes[2].Normalize3();

        // Right
        m_planes[3][0] = -viewProj[0][3] - viewProj[0][0];
        m_planes[3][1] = -viewProj[1][3] - viewProj[1][0];
        m_planes[3][2] = -viewProj[2][3] - viewProj[2][0];
        m_planes[3][3] = -viewProj[3][3] - viewProj[3][0];
        m_planes[3] = m_planes[3].Normalize3();

        // Top
        m_planes[4][0] = -viewProj[0][3] + viewProj[0][1];
        m_planes[4][1] = -viewProj[1][3] + viewProj[1][1];
        m_planes[4][2] = -viewProj[2][3] + viewProj[2][1];
        m_planes[4][3] = -viewProj[3][3] + viewProj[3][1];
        m_planes[4] = m_planes[4].Normalize3();

        // Bottom
        m_planes[5][0] = -viewProj[0][3] - viewProj[0][1];
        m_planes[5][1] = -viewProj[1][3] - viewProj[1][1];
        m_planes[5][2] = -viewProj[2][3] - viewProj[2][1];
        m_planes[5][3] = -viewProj[3][3] - viewProj[3][1];
        m_planes[5] = m_planes[5].Normalize3();
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

    bool Frustum::CheckSphere(const Vector4& center, float radius) const
    {
        BLK_ASSERT(center.w() == 1.0f);

        for (const auto& plane : m_planes)
        {
            if (plane.Dot(center) < -radius)
            {
                return false;
            }
        }

        return true;
    }

    Frustum::TestResult Frustum::CheckAABB(const AABB& boundingBox) const
    {
        TestResult result = Inside;

        for (const auto& plane : m_planes)
        {
            Vector4 min, max;
            min.w() = max.w() = 1.0f;

            for (size_t i = 0; i < 3; ++i)
            {
                if (plane[i] > 0)
                {
                    min[i] = boundingBox.GetMin()[i];
                    max[i] = boundingBox.GetMax()[i];
                }
                else
                {
                    min[i] = boundingBox.GetMax()[i];
                    max[i] = boundingBox.GetMin()[i];
                }
            }

            if (plane.Dot(min) > 0.0f)
            {
                return Outside;
            }
            if (plane.Dot(max) > 0.0f)
            {
                result = Intersects;
            }
        }

        return result;
    }

} // namespace Boolka
