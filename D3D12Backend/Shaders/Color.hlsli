#ifndef __COLOR_HLSL__
#define __COLOR_HLSL__

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

#endif
