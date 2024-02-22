#include "../Color.hlsli"
#include "../FullScreen/FullScreenCommon.hlsli"

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

float TonemapViaCompute(float c)
{
    return LinearToPQ(GTTonemap(c));
}

float3 TonemapViaCompute(float3 c)
{
    return float3(TonemapViaCompute(c.r), TonemapViaCompute(c.g), TonemapViaCompute(c.b));
}

float TonemapLUTRemapForward(float c)
{
    c = c / 5.0f;
    c = sqrt(c);
    c = c * (float(BLK_TONEMAPPING_LUT_RESOLUTION - 1) / BLK_TONEMAPPING_LUT_RESOLUTION);
    c = c + 0.5f / BLK_TONEMAPPING_LUT_RESOLUTION;
    return saturate(c);
}

float TonemapLUTRemapBackward(float c)
{
    c = saturate(c);
    c = c - 0.5f / BLK_TONEMAPPING_LUT_RESOLUTION;
    c = c * (BLK_TONEMAPPING_LUT_RESOLUTION / float(BLK_TONEMAPPING_LUT_RESOLUTION - 1));
    c = c * c;
    return c * 5.0f;
}

float TonemapViaLUT(float c)
{
    return tonemappingLUT.SampleLevel(linearSingleMipSampler, TonemapLUTRemapForward(c), 0);
}

float3 TonemapViaLUT(float3 c)
{
    return float3(TonemapViaLUT(c.r), TonemapViaLUT(c.g), TonemapViaLUT(c.b));
}
