#include "../MeshCommon.hlsli"

struct PSOut
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
};

PSOut main(Vertex In)
{
    PSOut Out = (PSOut)0;
    Out.color = sceneTextures[In.materialID].Sample(anisoSampler, In.texcoord.xy);
    Out.normal = float4(In.normal, float(In.materialID));
    return Out;
}
