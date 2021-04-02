#include "DebugMeshPassShadersCommon.hlsli"

[NumThreads(1, 1, 1)]
[OutputTopology("triangle")] 
void main(out vertices VSOut vertices[3],
          out indices uint3 triangles[1])
{
    SetMeshOutputCounts(3, 1);

    static const float a = 0.8f;
    static const float2 positions[3] = 
    {
        float2(-sqrt(3.0f) / 2.0f * a, -a / 2.0f),
        float2(sqrt(3.0f) / 2.0f * a, -a / 2.0f), 
        float2(0.0f, a)
    };

    vertices[0].position = float4(mul(positions[0], vertexToScreen), 1, 1);
    vertices[0].color = float4(1, 0, 0, 0);
    vertices[1].position = float4(mul(positions[1], vertexToScreen), 1, 1);
    vertices[1].color = float4(0, 1, 0, 0);
    vertices[2].position = float4(mul(positions[2], vertexToScreen), 1, 1);
    vertices[2].color = float4(0, 0, 1, 0);

    triangles[0] = uint3(0, 1, 2);
}