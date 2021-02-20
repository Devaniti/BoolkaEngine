#include "../FullScreen/FullScreenCommon.hlsli"
#include "../Color.hlsli"

Texture2D<float4> lightBuffer : register(t3);
SamplerState pointSampler : register(s0);

struct PSOut
{
    float4 color : SV_Target;
};

// https://www.desmos.com/calculator/gslcdxvipg
float GTTonemap(float x)
{
    float P = 1.0f;
    float a = 1.0f;
    float m = 0.22f;
    float l = 0.4f;
    float c = 1.33f;
    float b = 0.0f;
    float l0 = (P - m) * l / a;
    float L0 = m - m / a;
    float L1 = m + (1.0f- m) / a;
    float Lx = m + a * (x - m);
    float Tx = m * pow(x / m, c) + b;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = a * P / (P - S1);
    float Sx = P - (P - S1) * exp(-C2 * (x - S0) / P);
    float w0 = 1.0f - smoothstep(0, m, x);
    float w2 = step(m + l0, x);
    float w1 = 1.0f - w0 - w2;
    return Tx * w0 + Lx * w1 + Sx * w2;
}

float3 GTTonemap(float3 c)
{
    // Another option is to tonemap max from rgb components and scale others accordingly
    return float3(GTTonemap(c.r), GTTonemap(c.g), GTTonemap(c.b));
}

float3 Tonemap(float3 c)
{
    return LinearToSRGB(GTTonemap(c));
}

PSOut main(VSOut In)
{
    PSOut Out = (PSOut) 0;
    
    float2 UV = In.texcoord;
    float4 light = lightBuffer.Sample(pointSampler, UV);
    Out.color = float4(Tonemap(light.rgb), 0.0f);
    return Out;
}