#include "../Common.hlsli"

Texture2D<float4> sceneTextures[300] : register(t0, space1);

struct VSIn
{
    float3 position : POSITION;
    int materialID : MATERIAL;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VSOut
{
    float4 position : SV_Position;
    nointerpolation int materialID : MATERIAL;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PSOut
{
    float4 color : SV_Target;
};