struct VSIn
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VSOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

struct PSOut
{
    float4 color : SV_Target;
};

cbuffer PerFrame : register(b0)
{
    float4 vertexToScreenPacked;
};

static float2x2 vertexToScreen = 
{ 
    {vertexToScreenPacked[0], vertexToScreenPacked[1]}, 
    {vertexToScreenPacked[2], vertexToScreenPacked[3]} 
};
