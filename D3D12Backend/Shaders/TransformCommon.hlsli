#ifndef __TRANSFORM_COMMON_HLSL__
#define __TRANSFORM_COMMON_HLSL__

#include "Common.hlsli"

float3 CalculateViewPos(float2 UV, float depthVal)
{
    float4 viewProjPos = float4(UV.x * 2.0f - 1.0f, UV.y * -2.0f + 1.0f, depthVal, 1.0f);
    float4 viewPos = mul(viewProjPos, invProjMatrix);
    return viewPos.xyz / viewPos.w;
}

#endif