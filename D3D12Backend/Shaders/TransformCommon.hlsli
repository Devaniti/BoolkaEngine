#ifndef __TRANSFORM_COMMON_HLSLI__
#define __TRANSFORM_COMMON_HLSLI__

#include "Common.hlsli"

float3 CalculateViewPos(float2 UV, float depthVal)
{
    float4 viewProjPos = float4(UV.x * 2.0f - 1.0f, UV.y * -2.0f + 1.0f, depthVal, 1.0f);
    float4 viewPos = mul(viewProjPos, PerFrame.invProjMatrix);
    return viewPos.xyz / viewPos.w;
}

float3 CalculateWorldPos(float3 viewPos)
{
    return mul(float4(viewPos, 1.0f), PerFrame.invViewMatrix).xyz;
}

float3 CalculateWorldSpaceNormal(float3 vsNormal)
{
    return normalize(mul(float4(vsNormal, 0.0f), PerFrame.invViewMatrix).xyz);
}

#endif
