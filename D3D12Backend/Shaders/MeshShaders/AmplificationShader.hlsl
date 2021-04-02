#include "../MeshCommon.hlsli"

[numthreads(1, 1, 1)]
void main(uint gid : SV_GroupID) 
{
    const ObjectData objectData = GetObjectData(gid);
    bool visible = IntersectionFrustumAABB(mainViewFrustum, objectData.boundingBox);
    Payload payload = (Payload)0;
    payload.meshletOffset = objectData.meshletOffset;
    payload.materialID = objectData.materialIndex;
    DispatchMesh(objectData.meshletCount * visible, 1, 1, payload);
}
