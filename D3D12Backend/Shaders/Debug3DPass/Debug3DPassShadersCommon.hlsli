struct VSIn
{
    float3 position : POSITION;
    float3 color : COLOR;
    float4x4 worldMatrix : WORLD_MATRIX;
};

struct VSOut
{
    float4 position : SV_Position;
    float3 color : COLOR;
};

struct PSOut
{
    float4 color : SV_Target;
};

cbuffer PerFrame : register(b0)
{
    float4x4 viewProjectionMatrix;
};
