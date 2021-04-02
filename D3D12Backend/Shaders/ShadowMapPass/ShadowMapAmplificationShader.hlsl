#include "ShadowMapShadersCommon.hlsli"

[numthreads(1, 1, 1)] void main(uint gid : SV_GroupID)
{
    const ObjectData objectData = GetObjectData(gid);
    uint viewIndex = extraPassParam;
    bool visible = IntersectionFrustumAABB(shadowFrustum[viewIndex], objectData.boundingBox);
    SimplePayload payload = (SimplePayload)0;
    payload.meshletOffset = objectData.meshletOffset;
    DispatchMesh(objectData.meshletCount * visible, 1, 1, payload);
}
