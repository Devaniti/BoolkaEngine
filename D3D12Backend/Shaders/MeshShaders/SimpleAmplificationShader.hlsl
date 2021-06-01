#include "../MeshCommon.hlsli"

[numthreads(1, 1, 1)]
void main(uint gid : SV_GroupID) 
{
    const ObjectData objectData = GetObjectData(gid);
    bool visible = IntersectionFrustumAABB(PerFrame.mainViewFrustum, objectData.boundingBox);
    SimplePayload payload = (SimplePayload)0;
    payload.meshletOffset = objectData.meshletOffset;
    DispatchMesh(objectData.meshletCount * visible, 1, 1, payload);
}
