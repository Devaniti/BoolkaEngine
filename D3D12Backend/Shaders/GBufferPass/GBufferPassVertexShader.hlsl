#include "GBufferPassShadersCommon.hlsli"

VSOut main(VSIn In)
{
	VSOut Out = (VSOut)0;
	Out.position = mul(float4(In.position, 1.0f), viewProjMatrix);
	Out.materialID = In.materialID;
	Out.normal = normalize(mul(normalize(In.normal), (float3x3)viewMatrix));
	Out.texcoord = In.texcoord;
	return Out;
}
