#include "../Common.hlsli"

struct VSIn
{
    uint vertexID : SV_VertexID;
};

struct VSOut
{
    float4 position : SV_Position;
    float2 texcoord : TexCoord0;
};
