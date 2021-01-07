
struct VSIn
{
    float3 position : POSITION;
};

struct VSOut
{
    float4 position : SV_Position;
};

cbuffer PerFrame : register(b0)
{
    float4x4 viewProjectionMatrix;
    float4x4 viewMatrix;
};
