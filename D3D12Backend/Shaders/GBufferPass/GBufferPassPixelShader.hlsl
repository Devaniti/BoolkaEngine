#include "GBufferPassShadersCommon.hlsli"

#pragma warning( disable : 3571 )


PSOut main(VSOut In)
{
	PSOut Out = (PSOut)0;
	Out.color = sceneTextures[In.materialID].Sample(anisoSampler, In.texcoord.xy);
	return Out;
}
