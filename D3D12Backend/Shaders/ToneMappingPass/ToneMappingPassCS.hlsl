#include "ToneMappingPassCommon.hlsli"

[numthreads(BLK_TONEMAPPING_LUT_GENERATION_GROUP_SIZE, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint idx = DTid.x;
    float lutU = float(idx + 0.5f) / float(BLK_TONEMAPPING_LUT_RESOLUTION);
    float inputValue = TonemapLUTRemapBackward(lutU);
    tonemappingLUTUAV[idx] = TonemapViaCompute(inputValue);
}
