#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#include "CppShared.hlsli"
#include "ResourceBindings.hlsli"

float2 GetBackbufferResolution()
{
    return Frame.backbufferResolutionInvBackBufferResolution.xy;
}

float2 GetInvBackbufferResolution()
{
    return Frame.backbufferResolutionInvBackBufferResolution.zw;
}

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

enum DebugMarker
{
    DebugMarker_Test0,
    DebugMarker_Test1,
    DebugMarker_Test2,
    DebugMarker_Test3,
};

void MarkDebugEvent(DebugMarker marker)
{
    uint dummy;
    InterlockedAdd(debugMarkers[marker], 1, dummy);
}

#include "ResourceBindings.hlsli"

#endif
