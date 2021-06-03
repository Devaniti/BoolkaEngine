#ifndef __CPP_SHARED_HLSLI__
#define __CPP_SHARED_HLSLI__

#ifdef __cplusplus

using float4x4 = Matrix4x4;
using float2 = Vector2;
using float3 = Vector3;
using float4 = Vector4;
using uint2 = Vector2u;
using uint3 = Vector3u;
using uint4 = Vector4u;
// Frustum is type in Boolka namespace

#else

struct Frustum
{
    float4 planes[6];
};

#endif

struct PerFrameConstantBuffer
{
    float4x4 viewProjMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 invViewProjMatrix;
    float4x4 invViewMatrix;
    float4x4 invProjMatrix;
    Frustum mainViewFrustum;
    float4 cameraWorldPos;
    float4 backbufferResolutionInvBackBufferResolution;
};

struct LightData
{
    float4 viewPos_nearZ;
    float4 color_farZ;
};

struct SunData
{
    float4 lightDirVS;
    float4 color;
    float4x4 viewToShadow;
};

struct LightingDataConstantBuffer
{
    SunData sun;
    uint4 lightCount;
    LightData lights[4];
};

struct MaterialData
{
    float4 diffuse;
    float4 specular_specularExp;
};

struct RayDifferentialPart
{
    float3 dO;
    float3 dD;
};

struct RayDifferential
{
    RayDifferentialPart dx;
    RayDifferentialPart dy;
};

struct ReflectionPayload
{
    float4 color;
    RayDifferential rayDifferential;
};

#endif