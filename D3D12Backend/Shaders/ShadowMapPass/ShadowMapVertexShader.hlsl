#include "ShadowMapShadersCommon.hlsli"

VSOut main(VSIn In)
{
    VSOut Out = (VSOut) 0;
    Out.position = mul(float4(In.position, 1.0f), viewProjectionMatrix[viewIndex]);
    return Out;
}
