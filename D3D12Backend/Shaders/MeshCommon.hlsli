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

bool IsConeDegenerate(MeshletCullData cullData)
{
    return (cullData.NormalCone >> 24) == 0xff;
}

float4 UnpackCone(uint normalCone)
{
    float4 result;
    result.x = float((normalCone >> 0) & 0xFF);
    result.y = float((normalCone >> 8) & 0xFF);
    result.z = float((normalCone >> 16) & 0xFF);
    result.w = float((normalCone >> 24) & 0xFF);

    result = result / 255.0f;

    result.xyz = result.xyz * 2.0f - 1.0f;

    return result;
}

bool IsMeshletVisible(in uint meshletIndex, in uint viewIndex)
{
    if (meshletIndex == -1)
    {
        return false;
    }

    MeshletCullData cullData = meshletCullBuffer[meshletIndex];
    Frustum viewFrustum = GPUCulling.views[viewIndex];
    float3 cameraPos = GPUCulling.cameraPos[viewIndex].xyz;
    float4 boundingSphere = cullData.BoundingSphere;

    // Only empty meshlets should have such bounding sphere
    // And we currently have empty meshlets as padding
    if (all(boundingSphere == float4(0.0f, 0.0f, 0.0f, 1.0f)))
    {
        return false;
    }

    // Frustum culling
    if (!IntersectionFrustumSphere(viewFrustum, boundingSphere))
    {
        return false;
    }

    // Backface culling
    // Cull whole meshlet if there are no triangles that are front facing
    if (IsConeDegenerate(cullData))
        return true;

    float4 normalCone = UnpackCone(cullData.NormalCone);
    float3 axis = normalCone.xyz;
    float angle = normalCone.w;

    float3 sphereCenter = boundingSphere.xyz;
    float3 apex = sphereCenter - axis * cullData.ApexOffset;
    float3 view = normalize(sphereCenter - cameraPos);

    // The normal cone w-component stores -cos(angle + 90 deg)
    // This is the min dot product along the inverted axis from which all the meshlet's triangles
    // are backface
    if (dot(view, -axis) > angle)
    {
        return false;
    }

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