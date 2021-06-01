#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#include "CppShared.hlsli"

ConstantBuffer<PerFrameConstantBuffer> PerFrame : register(b0);

float2 GetBackbufferResolution()
{
    return PerFrame.backbufferResolutionInvBackBufferResolution.xy;
}

float2 GetInvBackbufferResolution()
{
    return PerFrame.backbufferResolutionInvBackBufferResolution.zw;
}

struct VertexData1
{
    float3 position;
    float texCoordX;
};

struct VertexData2
{
    float3 normal;
    float texCoordY;
};

#include "ResourceBindings.hlsli"

#endif
