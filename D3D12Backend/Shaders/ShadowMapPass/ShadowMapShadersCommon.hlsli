#include "../MeshCommon.hlsli"

cbuffer PerPass : register(b1)
{
    Frustum shadowFrustum[25];
    float4x4 viewProjShadowMatrix[25];
};
