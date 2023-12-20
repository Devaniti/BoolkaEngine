#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#include "CppShared.hlsli"
#include "ResourceBindings.hlsli"

#define FLT_MAX 3.402823466e+38F

bool IntersectionFrustumAABB(const in Frustum currentFrustum, const in AABB boundingBox)
{
    [unroll(6)] for (int i = 0; i < 6; ++i)
    {
        float4 mask = (currentFrustum.planes[i] > float4(0, 0, 0, 0));
        float4 min = lerp(boundingBox.min, boundingBox.max, mask);
        float dotMin = dot(currentFrustum.planes[i], min);

        if (dotMin < 0)
            return false;
    };

    return true;
}

bool IntersectionFrustumSphere(const in Frustum currentFrustum, const in float4 sphere)
{
    float4 center = float4(sphere.xyz, 1.0f);
    float radius = sphere.w;

    [unroll(6)] for (int i = 0; i < 6; ++i)
    {
        if (dot(currentFrustum.planes[i], center) < -radius)
            return false;
    };

    return true;
}

uint CeilToNextPowerOfTwo(uint value)
{
    uint rounded = 1l << (firstbithigh(value - 1) + 1);
    if (value == 0)
    {
        rounded = 0;
    }
    return rounded;
}

void DebugIncrementCounter(uint marker)
{
    uint dummy;
    InterlockedAdd(debugMarkers[marker], 1, dummy);
}

void DebugSetData(uint marker, uint data)
{
    debugMarkers[marker] = data;
}

void DebugSetData(uint marker, float data)
{
    debugMarkers[marker] = asuint(data);
}

void ProfileSetData(uint offset, uint data)
{
    profileMetrics[offset] = data;
}

void ProfileSetData(uint offset, float data)
{
    profileMetrics[offset] = asuint(data);
}

#include "ResourceBindings.hlsli"

#endif
