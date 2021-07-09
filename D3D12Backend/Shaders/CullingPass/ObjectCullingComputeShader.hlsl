#include "../Common.hlsli"

ConstantBuffer<CullingDataConstantBuffer> PerPass : register(b1);

[numthreads(32, 1, 1)] 
void main(uint3 DTid : SV_DispatchThreadID) 
{
    uint objectIndex = DTid.x;
    uint viewIndex = DTid.y;
    uint objectCount = PerPass.objectCount.x;

    if (objectIndex >= objectCount)
        return;

    ObjectData objectData = objectBuffer[objectIndex];
    Frustum frustum = PerPass.views[viewIndex];
    bool intersect = IntersectionFrustumAABB(frustum, objectData.boundingBox);

    uint uavOffset = (BLK_MAX_OBJECT_COUNT + 1) * 2 * viewIndex;

    if (intersect)
    {
        uint destIndex;
        InterlockedAdd(gpuCullingUAV[uavOffset], 1, destIndex);
        gpuCullingUAV[uavOffset + 2 + destIndex * 2] = objectIndex;
        float4 objectPos = (objectData.boundingBox.min + objectData.boundingBox.max) / 2.0f;
        float distanceToNearPlane = dot(frustum.planes[0], objectPos);
        gpuCullingUAV[uavOffset + 2 + destIndex * 2 + 1] = asuint(distanceToNearPlane);
    }
}
