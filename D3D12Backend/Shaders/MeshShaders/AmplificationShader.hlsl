#include "../MeshCommon.hlsli"

groupshared Payload payload;
groupshared uint visibleCount;

[numthreads(BLK_AS_GROUP_SIZE, 1, 1)]
void main(uint GTiD : SV_GroupThreadID, uint GiD : SV_GroupID) 
{
    if (GTiD == 0)
    {
        visibleCount = 0;
    }
    GroupMemoryBarrierWithGroupSync();

    uint mehsletIndex = gpuCullingMeshletIndices[GiD] + GTiD;
    bool isMeshletVisible = IsMeshletVisible(mehsletIndex, 0);
    uint waveVisibleIndex = WavePrefixCountBits(isMeshletVisible);
    uint waveVisibleCount = WaveActiveCountBits(isMeshletVisible);
    uint waveOffset;
    if (WaveIsFirstLane())
    {
        InterlockedAdd(visibleCount, waveVisibleCount, waveOffset);
    }
    waveOffset = WaveReadLaneFirst(waveOffset);
    uint threadIndex = waveOffset + waveVisibleIndex;

    if (isMeshletVisible)
    {
        payload.meshletIndicies[threadIndex] = mehsletIndex;
    }

    GroupMemoryBarrierWithGroupSync();
    DispatchMesh(visibleCount, 1, 1, payload);
}
