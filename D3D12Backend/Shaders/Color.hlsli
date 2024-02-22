#ifndef __COLOR_HLSLI__
#define __COLOR_HLSLI__

float LinearToSRGB(float linearColor)
{
    return (linearColor < 0.0031308 ? linearColor * 12.92
                                    : 1.055 * pow(linearColor, 0.41666) - 0.055);
}

float SRGBToLinear(float srgbColor)
{
    return (srgbColor <= 0.04045) ? srgbColor / 12.92 : pow((srgbColor + 0.055) / 1.055, 2.4);
}

float3 LinearToSRGB(float3 linearColor)
{
    return float3(LinearToSRGB(linearColor.r), LinearToSRGB(linearColor.g),
                  LinearToSRGB(linearColor.b));
}

float3 SRGBToLinear(float3 srgbColor)
{
    return float3(SRGBToLinear(srgbColor.r), SRGBToLinear(srgbColor.g), SRGBToLinear(srgbColor.b));
}

float LinearToPQ(float linearColor)
{
    linearColor = linearColor / 100.0f;
    float m1 = 2610.0 / 4096.0 / 4;
    float m2 = 2523.0 / 4096.0 * 128;
    float c1 = 3424.0 / 4096.0;
    float c2 = 2413.0 / 4096.0 * 32;
    float c3 = 2392.0 / 4096.0 * 32;
    float Lp = pow(linearColor, m1);
    return pow((c1 + c2 * Lp) / (1 + c3 * Lp), m2);
}


#endif
