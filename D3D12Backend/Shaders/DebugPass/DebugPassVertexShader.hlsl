#include "DebugPassShadersCommon.hlsli"

VSOut main(VSIn In)
{
    VSOut Out = (VSOut) 0;
    Out.position = float4(mul(In.position.xy, vertexToScreen), 1.0f, 1.0f);
    Out.color = In.color;
    return Out;
}
