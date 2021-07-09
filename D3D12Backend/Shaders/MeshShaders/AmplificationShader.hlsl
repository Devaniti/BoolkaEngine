#include "../MeshCommon.hlsli"

groupshared Payload payload;

[numthreads(32, 1, 1)]
void main(uint DTiD : SV_DispatchThreadID, uint GTiD : SV_GroupThreadID) 
{
    uint meshletOffset = DTiD;
    uint groupOffset = GTiD;

    uint mehsletIndex = gpuCullingMeshletIndiciesUAV[meshletIndirectionOffset + meshletOffset];
    bool isMeshletVisible = IsMeshletVisible(mehsletIndex, 1);

    if (isMeshletVisible)
    {
        uint groupVisibleIndex = WavePrefixCountBits(isMeshletVisible);
        payload.meshletIndicies[groupVisibleIndex] = mehsletIndex;
    }

    uint visibleCount = WaveActiveCountBits(isMeshletVisible);
    DispatchMesh(visibleCount, 1, 1, payload);
}
