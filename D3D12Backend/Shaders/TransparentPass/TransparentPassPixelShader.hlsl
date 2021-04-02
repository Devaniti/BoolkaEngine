#include "../Color.hlsli"
#include "../MeshCommon.hlsli"

struct PSOut
{
    float4 color : SV_Target;
};

PSOut main(Vertex In)
{
    PSOut Out = (PSOut)0;
    Out.color = sceneTextures[In.materialID].Sample(anisoSampler, In.texcoord.xy);
    clip(Out.color.a == 0.0f ? -1 : 1);
    Out.color.rgb = SRGBToLinear(Out.color.rgb);
    return Out;
}
