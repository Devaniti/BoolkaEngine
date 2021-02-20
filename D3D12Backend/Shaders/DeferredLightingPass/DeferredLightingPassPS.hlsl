#include "../FullScreen/FullScreenCommon.hlsli"
#include "../Color.hlsli"

Texture2D<float4> albedo : register(t0);
Texture2D<float4> normal : register(t1);
Texture2D<float> depth : register(t2);
SamplerState pointSampler : register(s0);

struct Light
{
    float4 viewPos;
    float4 color;
};

cbuffer DeferredPass : register(b1)
{
    Light lights[4];
    uint lightCount;
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

float3 CalculateDiffuse(float3 albedoVal, float3 normalVal, float3 viewPos)
{
    float3 result = 0.0f;
    
    [unroll(4)]
    for (uint i = 0; i < lightCount; ++i)
    {
        float3 lightVector = lights[i].viewPos.xyz - viewPos.xyz;
        float3 lightVectorSqr = lightVector * lightVector;
        float3 lightDir = normalize(lightVector);
        float angleMult = saturate(dot(lightDir, normalVal));
        float distSqr = lightVectorSqr.x + lightVectorSqr.y + lightVectorSqr.z;
        result += albedoVal * angleMult * lights[i].color.rgb / distSqr;
    }
    
    return result;
}

float3 CalculateSpecular(float3 specularVal, float3 normalVal, float3 viewPos)
{
    float3 result = 0.0f;
    
    float3 viewDir = normalize(viewPos);
    float3 reflectedLightVector = reflect(viewDir, normalVal);
    
    [unroll(4)]
    for (uint i = 0; i < lightCount; ++i)
    {
        float3 lightVector = lights[i].viewPos.xyz - viewPos.xyz;
        float3 lightVectorSqr = lightVector * lightVector;
        float3 lightDir = normalize(lightVector);
        float angleMult = pow(saturate(dot(reflectedLightVector, lightDir)), specExp);
        float distSqr = lightVectorSqr.x + lightVectorSqr.y + lightVectorSqr.z;
        result += specularVal * angleMult * lights[i].color.rgb / distSqr;
    }
    
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
    float3 viewPos = CalculateViewPos(UV, depthVal);
    float3 result = CalculateAmbient(albedoVal) + CalculateDiffuse(albedoVal, normalVal, viewPos) + CalculateSpecular(albedoVal, normalVal, viewPos);
    Out.light = float4(result, 0.0f);
    return Out;
}
