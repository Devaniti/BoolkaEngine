#include "ShadowMapShadersCommon.hlsli"

SimpleVertex GetSimpleVertexShadow(in const SimplePayload payload, in const MeshletData meshletData,
                                   in uint vertexIndex)
{
    SimpleVertex Out = (SimpleVertex)0;

    uint remappedVertexIndex = vertexIndirectionBuffer[meshletData.VertOffset + vertexIndex];
    VertexData1 vertexData1 = vertexBuffer1[remappedVertexIndex];

    uint viewIndex = extraPassParam;
    Out.position = mul(float4(vertexData1.position, 1.0f), viewProjShadowMatrix[viewIndex]);

    return Out;
}

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(uint gtid : SV_GroupThreadID,
          uint gid : SV_GroupID,
          in payload SimplePayload payload,
          out vertices SimpleVertex vertices[64],
          out indices uint3 triangles[126])
{
    MeshletData meshletData = GetMeshletData(payload, gid);

    SetMeshOutputCounts(meshletData.VertCount, meshletData.PrimCount);

    if (gtid < meshletData.VertCount)
    {
        vertices[gtid] = GetSimpleVertexShadow(payload, meshletData, gtid);
    }

    if (gtid < meshletData.PrimCount)
    {
        triangles[gtid] = GetPrimitive(payload, meshletData, gtid);
    }
}
