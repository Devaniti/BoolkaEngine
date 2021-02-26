
struct VSIn
{
    float3 position : POSITION;
};

struct VSOut
{
    float4 position : SV_Position;
};

cbuffer PerFrustum : register(b1)
{
    float4x4 viewProjectionMatrix[24];
};

cbuffer PerFrustum : register(b2)
{
    uint viewIndex;
};
