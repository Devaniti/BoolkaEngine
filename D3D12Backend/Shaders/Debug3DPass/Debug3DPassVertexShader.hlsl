#include "Debug3DPassShadersCommon.hlsli"

VSOut main(VSIn In)
{
    VSOut Out = (VSOut)0;
    Out.position = mul(mul(float4(In.position, 1.0f), In.worldMatrix), viewProjectionMatrix);
    Out.color = In.color;
    return Out;
}
