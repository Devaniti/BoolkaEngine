#include "GBufferPassShadersCommon.hlsli"

#pragma warning( disable : 3571 )


PSOut main(VSOut In)
{
	PSOut Out = (PSOut)0;
	Out.color = sceneTextures[In.materialID].Sample(anisoSampler, In.texcoord.xy);
	clip(Out.color.a == 0.0f ? -1 : 1);
	//Out.color = float4(In.texcoord.xy, 0.5f, 1.0f);
	return Out;
}
