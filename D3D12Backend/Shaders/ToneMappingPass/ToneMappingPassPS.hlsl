#include "ToneMappingPassCommon.hlsli"

PSOut main(VSOut In)
{
    PSOut Out = (PSOut)0;

    uint2 vpos = uint2(In.position.xy);
    float4 light = lightBuffer.Load(uint3(vpos, 0));
#ifdef BLK_TONEMAPPING_USE_LUT
    float3 tonemapped = TonemapViaLUT(light.rgb);
#else
    float3 tonemapped = TonemapViaCompute(light.rgb);
#endif
    Out.color = float4(tonemapped, 0.0f);
    return Out;
}
