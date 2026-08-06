#include "../TransformCommon.hlsli"
#include "../FullScreen/FullScreenCommon.hlsli"

struct PSOut
{
    float4 color : SV_Target0;
};

PSOut main(VSOut In)
{
    PSOut Out = (PSOut)0;

    float2 UV = In.texcoord;
    float3 viewPos = CalculateViewPos(UV, 1.0f);
    float3 viewDirWorld = mul(float4(viewPos, 0.0f), Frame.invViewMatrix).xyz;
    //TODO Implement post process shader with exposure
    Out.color = 3.0*skyBox.Sample(anisoSampler, viewDirWorld);
    return Out;
}
