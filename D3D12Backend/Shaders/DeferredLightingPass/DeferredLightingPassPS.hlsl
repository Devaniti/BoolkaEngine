#include "../Color.hlsli"
#include "../TransformCommon.hlsli"
#include "../FullScreen/FullScreenCommon.hlsli"
#include "../LightingLibrary/Lighting.hlsli"

struct PSOut
{
    float4 light : SV_Target;
};

PSOut main(VSOut In)
{
    PSOut Out = (PSOut)0;

    float2 UV = In.texcoord;
    uint2 vpos = uint2(In.position.xy);
    float3 albedoVal = SRGBToLinear(albedo.Load(uint3(vpos, 0)).rgb);
    float4 normalVal = normal.Load(uint3(vpos, 0));
    float3 rtVal = raytraceResults.Load(uint3(vpos, 0)).rgb;
    float depthVal = depth.Load(uint3(vpos, 0));

    float3 normal = normalVal.xyz;
    uint materialID = uint(normalVal.w);

    if (depthVal == 1.0f) // Far plane
        discard;          // Skip writting to RT, SkyBox will overwrite it

    float3 viewPos = CalculateViewPos(UV, depthVal);
    float3 viewDir = normalize(viewPos);

    MaterialData matData = materialsData[materialID];
    Out.light = float4(CalculateLighting(matData, albedoVal, albedoVal, normal, viewPos, viewDir), 0.0f);
    if (matData.specularExp > 200.0f)
        Out.light += float4(rtVal, 0.0f);

    return Out;
}
