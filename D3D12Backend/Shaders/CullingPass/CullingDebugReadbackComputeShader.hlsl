#include "../Common.hlsli"

ConstantBuffer<CullingDataConstantBuffer> PerPass : register(b1);

[numthreads(1, 1, 1)] 
void main(uint3 DTid : SV_DispatchThreadID) 
{
    uint viewIndex = DTid.x;

    uint uavOffset = (BLK_MAX_OBJECT_COUNT + 1) * 2 * viewIndex;

    gpuCullingDebugReadbackUAV[viewIndex * 2] = gpuCullingUAV[uavOffset];
    gpuCullingDebugReadbackUAV[viewIndex * 2 + 1] = gpuCullingUAV[uavOffset + 1];
}
