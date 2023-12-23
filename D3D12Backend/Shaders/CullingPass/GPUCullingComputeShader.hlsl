#include "../Common.hlsli"

#define BLK_GPU_CULLING_GROUP_SIZE (BLK_MAX_OBJECT_COUNT / 2)

ConstantBuffer<CullingDataConstantBuffer> PerPass : register(b1);

groupshared uint sharedMem[BLK_MAX_OBJECT_COUNT];
groupshared uint values[BLK_MAX_OBJECT_COUNT];
groupshared uint culledObjectCount;

void SetKey(uint index, float value)
{
    sharedMem[index] = asuint(value);
}

float GetKey(uint index)
{
    return asfloat(sharedMem[index]);
}

void SetPrefixSum(uint index, uint value)
{
    sharedMem[index] = value;
}

uint GetPrefixSum(uint index)
{
    return sharedMem[index];
}

void ProfileReportCullingStats(uint viewIndex, uint objectCount, uint meshletCount)
{
    ProfileSetData(BLK_PROFILING_DATA_GPU_CULLING_OFFSET + viewIndex * BLK_PROFILING_DATA_GPU_CULLING_VALUES_PER_VIEW + 0, objectCount);
    ProfileSetData(BLK_PROFILING_DATA_GPU_CULLING_OFFSET + viewIndex * BLK_PROFILING_DATA_GPU_CULLING_VALUES_PER_VIEW + 1, meshletCount);
}

void CullObjects(uint threadID, uint viewIndex)
{
    ObjectData objectData = objectBuffer[threadID];
    Frustum frustum = PerPass.views[viewIndex];
    bool intersect = IntersectionFrustumAABB(frustum, objectData.boundingBox);
    
    uint waveVisibleCount = WaveActiveSum(intersect);
    uint threadWaveOffset = WavePrefixSum(intersect);
    uint waveOffset = 0;
    if (WaveIsFirstLane())
    {
        InterlockedAdd(culledObjectCount, waveVisibleCount, waveOffset);
    }
    waveOffset = WaveReadLaneFirst(waveOffset);

    if (intersect)
    {
        uint offset = waveOffset + threadWaveOffset;
        float4 objectPos = (objectData.boundingBox.min + objectData.boundingBox.max) / 2.0f;
        float distanceToNearPlane = dot(frustum.planes[0], objectPos);
        SetKey(offset, distanceToNearPlane);
        values[offset] = threadID;
    }
}

// countToSort must be a power of 2
void BitonicSort(uint threadID, uint countToSort)
{
    for (uint k = 2; k <= countToSort; k <<= 1)
    {
        if (threadID <= (countToSort >> 1))
        {
            uint j = k >> 1;
            uint idx2 = ((threadID & (~(j - 1))) << 1) | j | (threadID & (j - 1));
            uint idx1 = idx2 ^ (k - 1);
            float key1 = GetKey(idx1);
            float key2 = GetKey(idx2);
            if (key1 > key2)
            {
                SetKey(idx1, key2);
                SetKey(idx2, key1);
                uint value1 = values[idx1];
                uint value2 = values[idx2];
                values[idx1] = value2;
                values[idx2] = value1;
            }
        }
        GroupMemoryBarrierWithGroupSync();
        for (uint j = k >> 2; j > 0; j >>= 1)
        {
            if (threadID <= (countToSort >> 1))
            {
                uint idx2 = ((threadID & (~(j - 1))) << 1) | j | (threadID & (j - 1));
                uint idx1 = idx2 ^ j;
                float key1 = GetKey(idx1);
                float key2 = GetKey(idx2);
                if (key1 > key2)
                {
                    SetKey(idx1, key2);
                    SetKey(idx2, key1);
                    uint value1 = values[idx1];
                    uint value2 = values[idx2];
                    values[idx1] = value2;
                    values[idx2] = value1;
                }
            }
            GroupMemoryBarrierWithGroupSync();
        }
    }
}

void PrefixSumFirstStep(uint threadID, uint maxCount)
{
    uint objectIndex = values[threadID];
    uint objectMeshletsCount = 0;
    if (objectIndex != -1)
    {
        objectMeshletsCount = objectBuffer[objectIndex].meshletCount / BLK_AS_GROUP_SIZE;
    }
    uint waveMeshletCountPrefix = WavePrefixSum(objectMeshletsCount) + objectMeshletsCount;
    SetPrefixSum(threadID, waveMeshletCountPrefix);
}

void PrefixSumReduceStep(uint threadID, uint maxCount, uint stride)
{
    uint idx = threadID * stride + stride - 1;
    if (idx >= maxCount)
    {
        return;
    }

    uint count = GetPrefixSum(idx);
    uint waveMeshletCountPrefix = WavePrefixSum(count) + count;
    SetPrefixSum(idx, waveMeshletCountPrefix);
}

void PrefixSumExpandStep(uint threadID, uint maxCount, uint stride)
{
    uint idx = threadID * stride + stride - 1;

    if (idx >= maxCount)
    {
        return;
    }

    uint prefix = 0;
    if (WaveIsFirstLane())
    {
        int prefixIdx = int(idx) - stride;
        if (prefixIdx >= 0)
        {
            prefix = GetPrefixSum(prefixIdx);
        }
    }
    prefix = WaveReadLaneFirst(prefix);
    uint count = GetPrefixSum(idx);
    count += prefix;
    if (WaveGetLaneIndex() != (WaveGetLaneCount() - 1))
    {
        SetPrefixSum(idx, count);
    }
}

uint GetTotalSum(uint culledObjectCount)
{
    return GetPrefixSum(max(culledObjectCount, 1) - 1);
}

void BuildPrefixSum(uint threadID, uint culledObjectCount)
{
    uint stride;
    
    {
        for (uint idx = threadID; idx < culledObjectCount; idx += BLK_GPU_CULLING_GROUP_SIZE)
        {
            PrefixSumFirstStep(idx, culledObjectCount);
        }
    }
    GroupMemoryBarrierWithGroupSync();

    for(stride = WaveGetLaneCount(); stride < culledObjectCount; stride *= WaveGetLaneCount())
    {
        PrefixSumReduceStep(threadID, culledObjectCount, stride);
        GroupMemoryBarrierWithGroupSync();
    }

    stride /= WaveGetLaneCount();
    for(stride /= WaveGetLaneCount(); stride >= WaveGetLaneCount(); stride /= WaveGetLaneCount())
    {
        PrefixSumExpandStep(threadID, culledObjectCount, stride);
        GroupMemoryBarrierWithGroupSync();
    }
    
    {
        for (uint idx = threadID; idx < culledObjectCount; idx += BLK_GPU_CULLING_GROUP_SIZE)
        {
            PrefixSumExpandStep(idx, culledObjectCount, 1);
        }
    }
    GroupMemoryBarrierWithGroupSync();
}

void ClearIndirectCommand(uint threadID, uint viewIndex)
{
    if (threadID == 0)
    {
        CullingCommandSignature cullingCommandSignature = (CullingCommandSignature)0;
        cullingCommandSignature.amplificationShaderGroups = uint3(0, 1, 1);
        gpuCullingCommandUAV[viewIndex] = cullingCommandSignature;
        ProfileReportCullingStats(viewIndex, 0, 0);
    }
}

void WriteIndirectCommand(uint threadID, uint viewIndex, uint culledObjectCount)
{
    if (threadID == 0)
    {
        uint totalASCount = GetTotalSum(culledObjectCount);
        CullingCommandSignature cullingCommandSignature = (CullingCommandSignature)0;
        cullingCommandSignature.amplificationShaderGroups = uint3(totalASCount, 1, 1);
        gpuCullingCommandUAV[viewIndex] = cullingCommandSignature;
        ProfileReportCullingStats(viewIndex, culledObjectCount, totalASCount * BLK_AS_GROUP_SIZE);
    }
}

void WriteMeshletOffsets(uint threadID, uint viewIndex)
{
    uint rangeStart = 0;
    if (threadID > 0)
    {
        rangeStart = GetPrefixSum(threadID - 1);
    }
    uint rangeEnd = GetPrefixSum(threadID);
    uint objectIndex = values[threadID];
    uint objectMeshletOffset = objectBuffer[objectIndex].meshletOffset;
    for (uint idx = rangeStart; idx < rangeEnd; idx++)
    {
        gpuCullingMeshletIndicesUAV[BLM_MAX_AS_GROUPS * viewIndex + idx] = objectMeshletOffset;
        objectMeshletOffset += BLK_AS_GROUP_SIZE;
    }
}

[numthreads(BLK_GPU_CULLING_GROUP_SIZE, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID) 
{
    uint threadID = GTid.x;
    uint viewIndex = Gid.x;
    uint totalObjectCount = PerPass.objectCount.x;

    if (threadID == 0)
    {
        culledObjectCount = 0;
    }
    SetKey(threadID, FLT_MAX);
    SetKey(threadID + BLK_GPU_CULLING_GROUP_SIZE, FLT_MAX);
    values[threadID] = -1;
    values[threadID + BLK_GPU_CULLING_GROUP_SIZE] = -1;

    GroupMemoryBarrierWithGroupSync();
    {
        [unroll(2)]
        for (uint idx = threadID; idx < totalObjectCount; idx += BLK_GPU_CULLING_GROUP_SIZE)
        {
            CullObjects(idx, viewIndex);
        }
    }
    GroupMemoryBarrierWithGroupSync();

    if (culledObjectCount == 0)
    {
        ClearIndirectCommand(threadID, viewIndex);
        return;
    }

    uint culledObjectCountRounded = CeilToNextPowerOfTwo(culledObjectCount);
    BitonicSort(threadID, culledObjectCountRounded);

    BuildPrefixSum(threadID, culledObjectCountRounded);

    WriteIndirectCommand(threadID, viewIndex, culledObjectCount);

    {
        for (uint idx = threadID; idx < culledObjectCount; idx += BLK_GPU_CULLING_GROUP_SIZE)
        {
            WriteMeshletOffsets(idx, viewIndex);
        }
    }
}
