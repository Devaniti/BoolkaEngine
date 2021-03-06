#include "GBufferPassShadersCommon.hlsli"

PSOut main(VSOut In)
{
    PSOut Out = (PSOut) 0;
    Out.color = sceneTextures[In.materialID].Sample(anisoSampler, In.texcoord.xy);
    Out.normal = float4(In.normal, 0.0f);
    return Out;
}
