#include "GBufferPassShadersCommon.hlsli"

#pragma warning( disable : 3571 )


PSOut main(VSOut In)
{
	PSOut Out = (PSOut)0;
	Out.color = sceneTextures[In.materialID].Sample(anisoSampler, In.texcoord.xy);
	//clip(Out.color.a == 0.0f ? -1 : 1);
	return Out;
}
