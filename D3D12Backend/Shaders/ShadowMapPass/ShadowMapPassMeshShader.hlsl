#include "../MeshCommon.hlsli"

Vertex GetVertexShadow(in const Payload payload, in const MeshletData meshletData,
                                   in uint vertexIndex)
{
    Vertex Out = (Vertex)0;

    uint remappedVertexIndex = vertexIndirectionBuffer[meshletData.VertOffset + vertexIndex];
    VertexData1 vertexData1 = vertexBuffer1[remappedVertexIndex];

    uint viewIndex = viewIndexParam;
    Out.position = mul(float4(vertexData1.position, 1.0f), GPUCulling.viewProjMatrix[viewIndex]);

    return Out;
}

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
        vertices[gtid] = GetVertexShadow(payload, meshletData, gtid);
    }

    if (gtid < meshletData.PrimCount)
    {
        triangles[gtid] = GetPrimitive(payload, meshletData, gtid);
    }
}
