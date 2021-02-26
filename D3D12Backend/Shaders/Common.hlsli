
cbuffer PerFrame : register(b0)
{
    float4x4 viewProjMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 invViewProjMatrix;
    float4x4 invViewMatrix;
    float4x4 invProjMatrix;
};

SamplerState pointSampler : register(s0);
SamplerState linearSingleMipSampler : register(s1);
SamplerState linearSampler : register(s2);
SamplerState anisoSampler : register(s3);
SamplerComparisonState shadowSampler : register(s4);
