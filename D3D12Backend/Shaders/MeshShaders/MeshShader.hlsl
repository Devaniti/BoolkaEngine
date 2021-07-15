#include "../MeshCommon.hlsli"

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(uint gtid : SV_GroupThreadID, 
          uint gid : SV_GroupID,
          in payload Payload payload,
          out vertices Vertex vertices[64],
          out indices uint3 triangles[126])
{
    MeshletData meshletData = GetMeshletData(payload, gid);

    SetMeshOutputCounts(meshletData.VertCount, meshletData.PrimCount);
    
    if (gtid < meshletData.VertCount)
    {
        vertices[gtid] = GetVertex(payload, meshletData, gtid);
    }
    
    if (gtid < meshletData.PrimCount)
    {
        triangles[gtid] = GetPrimitive(payload, meshletData, gtid);
    }
}
