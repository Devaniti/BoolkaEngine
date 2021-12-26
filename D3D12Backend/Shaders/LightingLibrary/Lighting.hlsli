#ifndef __LIGHTING_HLSLI__
#define __LIGHTING_HLSLI__

#include "../TransformCommon.hlsli"
#include "../CppShared.hlsli"

ConstantBuffer<LightingDataConstantBuffer> LightingData : register(b1);

static const float ambientLight = 0.1f;

float3 CalculateAmbient(MaterialData matData, float3 albedoVal)
{
    return albedoVal * ambientLight * matData.diffuse * matData.transparency;
}

float VectorToDepth(float3 vec, float n, float f)
{
    float3 AbsVec = abs(vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float CalculatePointLightShadow(uint lightIndex, float3 lightVector)
{
    float4 samplePos = mul(float4(-lightVector, 0.0f), Frame.invViewMatrix);
    float comparisonValue =
        VectorToDepth(samplePos.xyz, LightingData.lights[lightIndex].nearZ,
                      LightingData.lights[lightIndex].farZ);
    return shadowMapCube[lightIndex].SampleCmpLevelZero(shadowSampler, samplePos.xyz,
                                                        comparisonValue);
}

float3 CalculatePointLight(MaterialData matData, uint lightIndex, float3 albedoVal,
                           float3 specularVal, float3 normalVal,
                           float3 viewPos, float3 viewDir)
{
    float3 result = 0.0f;

    float3 lightVector = LightingData.lights[lightIndex].viewPos - viewPos;
    float3 lightDir = normalize(lightVector);
    float3 lightVectorSqr = lightVector * lightVector;
    float distSqr = lightVectorSqr.x + lightVectorSqr.y + lightVectorSqr.z;
    float farZ = LightingData.lights[lightIndex].farZ;
    // Non physically correct attenuation
    // This is needed to limit light radius
    float distanceAttenuation = (1.0f - distSqr / (farZ * farZ)) / distSqr;
    float NdotL = dot(lightDir, normalVal);
    if (NdotL <= 0.0f || distanceAttenuation <= 0.0f)
        return 0.0;

    float shadowFactor = CalculatePointLightShadow(lightIndex, lightVector);
    if (shadowFactor <= 0.0f)
        return 0.0;

    // Diffuse
    result += albedoVal * NdotL * matData.diffuse;

    float3 reflectedLightVector = reflect(viewDir, normalVal);
    float specularRefl =
        pow(saturate(dot(reflectedLightVector, lightDir)), matData.specularExp);
    // Specular
    result += specularVal * specularRefl * matData.specular;

    result *= LightingData.lights[lightIndex].color * shadowFactor * distanceAttenuation;
    return result;
}

float CalculateSunShadow(float3 viewPos)
{
    static const float shadowBias = 0.003f;
    float4 samplePos = mul(float4(viewPos, 1.0f), LightingData.sun.viewToShadow);
    float comparisonValue = samplePos.z - shadowBias;
    return shadowMapSun.SampleCmpLevelZero(shadowSampler, samplePos.xy, comparisonValue);
}

float3 CalculateSun(MaterialData matData, float3 albedoVal, float3 specularVal, float3 normalVal,
                    float3 viewPos, float3 viewDir)
{
    float3 result = 0.0f;

    float3 lightDir = LightingData.sun.lightDirVS.xyz;
    float NdotL = dot(lightDir, normalVal);

    if (NdotL <= 0.0f)
        return 0.0f;

    float shadowFactor = CalculateSunShadow(viewPos);
    if (shadowFactor <= 0.0f)
        return 0.0f;

    // Diffuse
    result += albedoVal * NdotL * matData.diffuse * matData.transparency;

    float3 reflectedLightVector = reflect(viewDir, normalVal);
    float specularRefl =
        pow(saturate(dot(reflectedLightVector, lightDir)), matData.specularExp);
    // Specular
    result += specularVal * specularRefl * matData.specular;

    result *= LightingData.sun.color.rgb * shadowFactor;
    return result;
}

float3 CalculateLighting(MaterialData matData, float3 albedoVal, float3 specularVal,
                         float3 normalVal, float3 viewPos, float3 viewDir)
{
    float3 result = CalculateAmbient(matData, albedoVal);
    result += CalculateSun(matData, albedoVal, specularVal, normalVal, viewPos, viewDir);
    [unroll(4)] for (uint i = 0; i < LightingData.lightCount.x; ++i)
    {
        result += CalculatePointLight(matData, i, albedoVal, specularVal, normalVal, viewPos, viewDir);
    }
    return result;
}

#endif
