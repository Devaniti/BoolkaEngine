#ifndef __MESH_COMMON_HLSLI__
#define __MESH_COMMON_HLSLI__

#include "Common.hlsli"

cbuffer PassConstData : register(b2)
{
    uint viewIndexParam;
};

cbuffer IndirectConstData : register(b3)
{
    uint meshletIndirectionOffset;
};

struct Vertex
{
    float4 position : SV_Position;
    nointerpolation int materialID : MATERIAL;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct Payload
{
    uint meshletIndicies[32];
};

bool IsMeshletVisible(in uint meshletIndex, in uint viewIndex)
{
    if (meshletIndex == -1)
    {
        return false;
    }

    MeshletCullData cullData = meshletCullBuffer[meshletIndex];
    Frustum viewFrustum = GPUCulling.views[viewIndex];

    if (!IntersectionFrustumSphere(viewFrustum, cullData.BoundingSphere))
        return false;

    return true;
}

MeshletData GetMeshletData(in const Payload payload, in uint groupID)
{
    MeshletData Out = (MeshletData)0;

    uint meshletIndex = payload.meshletIndicies[groupID];

    Out = meshletBuffer[meshletIndex];

    return Out;
}

Vertex GetVertex(in const Payload payload, in const MeshletData meshletData, in uint vertexIndex)
{
    Vertex Out = (Vertex)0;

    uint remappedVertexIndex = vertexIndirectionBuffer[meshletData.VertOffset + vertexIndex];
    VertexData1 vertexData1 = vertexBuffer1[remappedVertexIndex];
    VertexData2 vertexData2 = vertexBuffer2[remappedVertexIndex];

    Out.materialID = meshletData.MaterialID;
    Out.position = mul(float4(vertexData1.position, 1.0f), Frame.viewProjMatrix);
    Out.normal = normalize(mul(normalize(vertexData2.normal), (float3x3)Frame.viewMatrix));
    Out.texcoord = float2(vertexData1.texCoordX, vertexData2.texCoordY);

    float3 projPos = Out.position.xyz / Out.position.w;

    return Out;
}

uint3 UnpackPrimitive(uint primitive)
{
    return uint3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

uint3 GetPrimitive(in const Payload payload, in const MeshletData meshletData,
                   in uint primitiveIndex)
{
    return UnpackPrimitive(indexBuffer[meshletData.PrimOffset + primitiveIndex]);
}

#endif