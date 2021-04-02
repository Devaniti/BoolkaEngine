
#include "FullScreenCommon.hlsli"

VSOut main(VSIn In)
{
    VSOut Out = (VSOut)0;
    Out.texcoord = float2(In.vertexID & 2, (In.vertexID << 1) & 2);
    Out.position = float4(Out.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return Out;
}
