#ifndef __MESH_COMMON_HLSLI__
#define __MESH_COMMON_HLSLI__

#include "Common.hlsli"

struct MeshletData
{
    uint VertCount;
    uint VertOffset;
    uint PrimCount;
    uint PrimOffset;
};

struct AABB
{
    float4 min;
    float4 max;
};

struct ObjectData
{
    AABB boundingBox;

    uint materialIndex;
    uint meshletOffset;
    uint meshletCount;
    uint unused;
};

StructuredBuffer<uint> vertexIndirectionBuffer : register(t2, space2);
StructuredBuffer<uint> indexBuffer : register(t3, space2);
StructuredBuffer<MeshletData> meshletBuffer : register(t4, space2);
StructuredBuffer<ObjectData> objectBuffer : register(t5, space2);

cbuffer MeshletPassData : register(b2)
{
    uint objectOffset;
    uint extraPassParam;
};

struct Vertex
{
    float4 position : SV_Position;
    nointerpolation int materialID : MATERIAL;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct SimpleVertex
{
    float4 position : SV_Position;
};

struct Payload
{
    uint meshletOffset;
    uint materialID;
};

struct SimplePayload
{
    uint meshletOffset;
};

ObjectData GetObjectData(in uint objectIndex)
{
    ObjectData Out = (ObjectData)0;

    Out = objectBuffer[objectOffset + objectIndex];

    return Out;
}

bool IntersectionFrustumAABB(const in Frustum currentFrustum, const in AABB boundingBox)
{
    // Batch by 2 planes
    [unroll(6)] for (int i = 0; i < 6; ++i)
    {
        float4 mask = (currentFrustum.planes[i] > float4(0, 0, 0, 0));

        float4 min = lerp(boundingBox.max, boundingBox.min, mask);

        float dotMin = dot(currentFrustum.planes[i], min);

        if (dotMin < 0)
            return false;
    };

    return true;
}

MeshletData GetMeshletData(in const Payload payload, in uint meshletIndex)
{
    MeshletData Out = (MeshletData)0;

    Out = meshletBuffer[payload.meshletOffset + meshletIndex];

    return Out;
}

MeshletData GetMeshletData(in const SimplePayload payload, in uint meshletIndex)
{
    MeshletData Out = (MeshletData)0;

    Out = meshletBuffer[payload.meshletOffset + meshletIndex];

    return Out;
}

Vertex GetVertex(in const Payload payload, in const MeshletData meshletData, in uint vertexIndex)
{
    Vertex Out = (Vertex)0;

    uint remappedVertexIndex = vertexIndirectionBuffer[meshletData.VertOffset + vertexIndex];
    VertexData1 vertexData1 = vertexBuffer1[remappedVertexIndex];
    VertexData2 vertexData2 = vertexBuffer2[remappedVertexIndex];

    Out.materialID = payload.materialID;
    Out.position = mul(float4(vertexData1.position, 1.0f), PerFrame.viewProjMatrix);
    Out.normal = normalize(mul(normalize(vertexData2.normal), (float3x3)PerFrame.viewMatrix));
    Out.texcoord = float2(vertexData1.texCoordX, vertexData2.texCoordY);

    return Out;
}

SimpleVertex GetSimpleVertex(in const SimplePayload payload, in const MeshletData meshletData,
                             in uint vertexIndex)
{
    SimpleVertex Out = (SimpleVertex)0;

    uint remappedVertexIndex = vertexIndirectionBuffer[meshletData.VertOffset + vertexIndex];
    VertexData1 vertexData1 = vertexBuffer1[remappedVertexIndex];

    Out.position = mul(float4(vertexData1.position, 1.0f), PerFrame.viewProjMatrix);

    return Out;
}

uint3 UnpackPrimitive(uint primitive)
{
    return uint3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

uint3 GetPrimitive(in const SimplePayload payload, in const MeshletData meshletData,
                   in uint primitiveIndex)
{
    return UnpackPrimitive(indexBuffer[meshletData.PrimOffset + primitiveIndex]);
}

uint3 GetPrimitive(in const Payload payload, in const MeshletData meshletData,
                   in uint primitiveIndex)
{
    return UnpackPrimitive(indexBuffer[meshletData.PrimOffset + primitiveIndex]);
}

#endif