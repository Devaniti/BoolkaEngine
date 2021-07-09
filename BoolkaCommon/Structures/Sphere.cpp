#include "stdafx.h"

#include "Sphere.h"

namespace Boolka
{

    Sphere::Sphere(const Vector3& center, float radius)
        : m_Sphere(center, radius)
    {
    }

    Sphere::Sphere(const Vector4& sphere)
        : m_Sphere(sphere)
    {
    }

    Vector4 CalculateMaxDistancePoint(const Vector4& basePoint, const Vector4* verticies,
                                      size_t vertexCount)
    {
        float maxDistanceSqr = 0.0f;
        Vector4 result = basePoint;

        for (size_t i = 0; i < vertexCount; ++i)
        {
            const Vector4& currentVertex = verticies[i];

            float currentDistanceSqr = (basePoint - currentVertex).Length3Sqr();
            if (currentDistanceSqr > maxDistanceSqr)
            {
                result = currentVertex;
                maxDistanceSqr = currentDistanceSqr;
            }
        }

        return result;
    }

    // Extends sphere to include old version of sphere and an addPoint
    void ExtendSphere(Sphere& sphere, const Vector4& addPoint)
    {
        const Vector4& oldSphereCenter = sphere.GetData();
        float oldSphereRadiusSqr = sphere.GetData().w();

        float distanceToCenterSqr = (oldSphereCenter - addPoint).Length3Sqr();

        if (distanceToCenterSqr > oldSphereRadiusSqr)
        {
            float oldSphereRadius = ::sqrt(oldSphereRadiusSqr);
            float distanceToCenter = ::sqrt(distanceToCenterSqr);
            float newSphereRadius = (oldSphereRadius + distanceToCenter) / 2.0f;

            Vector4 appPointToCenterVector = (oldSphereCenter - addPoint).Normalize3();
            Vector4 newSphereCenter = addPoint + appPointToCenterVector * newSphereRadius;
            sphere = Sphere(newSphereCenter, newSphereRadius * newSphereRadius);
        }
    }

    // Approximate algorithm https://en.wikipedia.org/wiki/Bounding_sphere#Ritter's_bounding_sphere
    Sphere Sphere::BuildBoundingSphere(const Vector4* verticies, size_t vertexCount)
    {
        BLK_ASSERT(vertexCount > 0);
        Vector4 basePoint = verticies[0];

        Vector4 pivot1 = CalculateMaxDistancePoint(basePoint, verticies, vertexCount);
        Vector4 pivot2 = CalculateMaxDistancePoint(pivot1, verticies, vertexCount);

        Sphere result = Sphere((pivot1 + pivot2) / 2.0f, (pivot1 - pivot2).Length3Sqr());

        for (size_t i = 0; i < vertexCount; ++i)
        {
            const Vector4& currentVertex = verticies[i];
            ExtendSphere(result, currentVertex);
        }

        return result;
    }

    const Vector4& Sphere::GetData()
    {
        return m_Sphere;
    }

} // namespace Boolka
