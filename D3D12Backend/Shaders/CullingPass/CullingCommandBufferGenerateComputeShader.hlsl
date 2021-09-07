#include "../Common.hlsli"

ConstantBuffer<CullingDataConstantBuffer> PerPass : register(b1);

groupshared CullingCommandSignature Command;
groupshared uint meshletBufferOffset;

[numthreads(32, 1, 1)] 
void main(uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID) 
{
    uint objectIndex = DTid.x;
    uint viewIndex = DTid.y;
    uint commandBufferIndex = ((BLK_MAX_OBJECT_COUNT + 31) / 32) * viewIndex + Gid.x;
    uint objectUAVOffset = (BLK_MAX_OBJECT_COUNT + 1) * 2 * viewIndex;

    uint visibleObjectCount = gpuCullingUAV[objectUAVOffset];

    if (objectIndex >= visibleObjectCount)
        return;

    uint remappedIndex = gpuCullingUAV[objectUAVOffset + 2 + objectIndex * 2];
    ObjectData objectData = objectBuffer[remappedIndex];

    // round up to multiple of 32 since we process batches of 32 in amplification shader
    uint meshletCount = objectData.meshletCount;
    uint roundedMeshletCount = (objectData.meshletCount + 31) & ~uint(31);
    uint totalMeshletCount = WaveActiveSum(roundedMeshletCount);
    uint localMeshletDestOffset = WavePrefixSum(roundedMeshletCount);
    
    if (GTid.x == 0)
    {
        InterlockedAdd(gpuCullingUAV[objectUAVOffset + 1], totalMeshletCount, meshletBufferOffset);
        meshletBufferOffset += viewIndex * BLK_MAX_MESHLETS;

        Command.meshletOffset = meshletBufferOffset;
        Command.amplificationShaderGroups.x = totalMeshletCount / 32;
        Command.amplificationShaderGroups.yz = uint2(1, 1);

        gpuCullingCommandUAV[commandBufferIndex] = Command;
    }

    uint globalMeshDestOffset = meshletBufferOffset + localMeshletDestOffset;

    uint i;
    for (i = 0; i < meshletCount; i++)
    {
        gpuCullingMeshletIndiciesUAV[globalMeshDestOffset + i] = objectData.meshletOffset + i;
    }
    for (i = meshletCount; i < roundedMeshletCount; i++)
    {
        gpuCullingMeshletIndiciesUAV[globalMeshDestOffset + i] = -1;
    }

}
