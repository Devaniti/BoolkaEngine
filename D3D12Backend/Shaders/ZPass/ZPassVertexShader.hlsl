#include "ZPassShadersCommon.hlsli"

VSOut main(VSIn In)
{
	VSOut Out = (VSOut)0;
    Out.position = mul(float4(In.position, 1.0f), viewProjMatrix);
	return Out;
}
