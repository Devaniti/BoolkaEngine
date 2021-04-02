#include "Debug3DPassShadersCommon.hlsli"

PSOut main(VSOut In)
{
    PSOut Out = (PSOut)0;
    Out.color = float4(In.color, 1.0f);
    return Out;
}
