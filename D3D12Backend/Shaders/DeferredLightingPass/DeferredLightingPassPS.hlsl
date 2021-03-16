#include "../FullScreen/FullScreenCommon.hlsli"
#include "../Color.hlsli"

Texture2D<float4> albedo : register(t0);
Texture2D<float4> normal : register(t1);
Texture2D<float> depth : register(t2);
TextureCube<float> shadowMapCube[4] : register(t4);
Texture2D<float> shadowMapSun : register(t8);

struct Light
{
    float4 viewPos_nearZ;
    float4 color_farZ;
};

struct Sun
{
    float4 lightDirVS;
    float4 color;
    float4x4 viewToShadow;
};

cbuffer DeferredPass : register(b1)
{
    Light lights[4];
    uint lightCount;
    Sun sun;
};

struct PSOut
{
    float4 light : SV_Target;
};

static const float ambientLight = 0.1f;
static const float specExp = 10.0f; // TODO implement per material specular exponent

float3 CalculateAmbient(float3 albedoVal)
{
    return albedoVal * ambientLight;
}

float3 CalculateViewPos(float2 UV, float depthVal)
{
    float4 viewProjPos = float4(UV.x * 2.0f - 1.0f, UV.y * -2.0f + 1.0f, depthVal, 1.0f);
    float4 viewPos = mul(viewProjPos, invProjMatrix);
    return viewPos.xyz / viewPos.w;
}

float VectorToDepth(float3 vec, float n, float f)
{
    float3 AbsVec = abs(vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float CalculateLightShadow(uint lightIndex, float3 lightVector)
{
    static const float shadowBias = 0.001f;
    float4 samplePos = mul(float4(-lightVector, 0.0f), invViewMatrix);
    float comparisonValue = VectorToDepth(samplePos.xyz, lights[lightIndex].viewPos_nearZ.w, lights[lightIndex].color_farZ.w) - shadowBias;
    return shadowMapCube[lightIndex].SampleCmp(shadowSampler, samplePos.xyz, comparisonValue);
}

float3 CalculateLight(uint lightIndex, float3 albedoVal, float3 specularVal, float3 normalVal, float3 viewPos)
{
    float3 result = 0.0f;
    
    float3 lightVector = lights[lightIndex].viewPos_nearZ.xyz - viewPos;
    float3 lightDir = normalize(lightVector);
    float3 lightVectorSqr = lightVector * lightVector;
    float distSqr = lightVectorSqr.x + lightVectorSqr.y + lightVectorSqr.z;
    float farZ = lights[lightIndex].color_farZ.w;
    // Non physically correct attenuation
    // This is needed to limit light radius
    float distanceAttenuation = (1.0f - distSqr / (farZ * farZ)) / distSqr;
    float NdotL = dot(lightDir, normalVal);
    if (NdotL <= 0.0f || distanceAttenuation <= 0.0f)
        return 0.0;
    
    float shadowFactor = CalculateLightShadow(lightIndex, lightVector);
    if (shadowFactor <= 0.0f)
        return 0.0;
    
    // Diffuse
    result += albedoVal * NdotL;
    
    float3 viewDir = normalize(viewPos);
    float3 reflectedLightVector = reflect(viewDir, normalVal);
    float specularRefl = pow(saturate(dot(reflectedLightVector, lightDir)), specExp);
    // Specular
    result += specularVal * specularRefl;
    
    result *= lights[lightIndex].color_farZ.rgb * shadowFactor * distanceAttenuation;
    return result;
}

float CalculateSunShadow(float3 viewPos)
{
    static const float shadowBias = 0.003f;
    float4 samplePos = mul(float4(viewPos, 1.0f), sun.viewToShadow);
    float comparisonValue = samplePos.z - shadowBias;
    return shadowMapSun.SampleCmp(shadowSampler, samplePos.xy, comparisonValue);
}


float3 CalculateSun(float3 albedoVal, float3 specularVal, float3 normalVal, float3 viewPos)
{
    float3 result = 0.0f;
    
    float3 lightDir = sun.lightDirVS.xyz;
    float NdotL = dot(lightDir, normalVal);
    
    if (NdotL <= 0.0f)
        return 0.0f;
    
    float shadowFactor = CalculateSunShadow(viewPos);
    if (shadowFactor <= 0.0f)
        return 0.0f;
    
    // Diffuse
    result += albedoVal * NdotL;
    
    float3 viewDir = normalize(viewPos);
    float3 reflectedLightVector = reflect(viewDir, normalVal);
    float specularRefl = pow(saturate(dot(reflectedLightVector, lightDir)), specExp);
    // Specular
    result += specularVal * specularRefl;
    
    result *= sun.color.rgb * shadowFactor;
    return result;
}

PSOut main(VSOut In)
{
    PSOut Out = (PSOut) 0;
    
    float2 UV = In.texcoord;
    uint2 vpos = uint2(In.position.xy);
    float3 albedoVal = SRGBToLinear(albedo.Load(uint3(vpos, 0)).rgb);
    float3 normalVal = normal.Load(uint3(vpos, 0)).xyz;
    float depthVal = depth.Load(uint3(vpos, 0));
    if (depthVal == 1.0f) // Far plane
        return Out; // Clear far plane in shader for now
    float3 viewPos = CalculateViewPos(UV, depthVal);
    float3 result = CalculateAmbient(albedoVal);
    result += CalculateSun(albedoVal, albedoVal, normalVal, viewPos);
    [unroll(4)]
    for (uint i = 0; i < lightCount; ++i)
    {
        result += CalculateLight(i, albedoVal, albedoVal, normalVal, viewPos);
    }
    Out.light = float4(result, 0.0f);
    return Out;
}
