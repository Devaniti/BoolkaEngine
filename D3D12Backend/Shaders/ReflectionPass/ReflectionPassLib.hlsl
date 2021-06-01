#include "../Common.hlsli"
#include "../LightingLibrary/Lighting.hlsli"
#include "../Color.hlsli"

struct Payload
{
    float4 color;
};

// Calculate rays from camera
// Can be usefull for debugging
//inline void CalculateRay(uint2 vpos, out float3 origin, out float3 direction, out bool needCast)
//{
//    needCast = true;
//
//    float2 UV = vpos * GetInvBackbufferResolution();
//    float4 viewProjPos = float4(UV.x * 2.0f - 1.0f, UV.y * -2.0f + 1.0f, 0.0f, 1.0f);
//
//    float4 worldPos = mul(viewProjPos, PerFrame.invViewProjMatrix);
//
//    worldPos.xyz /= worldPos.w;
//    origin = PerFrame.cameraWorldPos.xyz;
//    direction = normalize(worldPos.xyz - origin);
//}

inline void CalculateRay(uint2 vpos, out float3 origin, out float3 direction, out bool needCast)
{
    float depthVal = depth.Load(uint3(vpos, 0));
    float4 normalVal = normal.Load(uint3(vpos, 0));
    float3 normal = normalVal.xyz;
    uint materialID = uint(normalVal.w);

    float2 UV = vpos * GetInvBackbufferResolution();

    if (depthVal == 1.0f) // Far plane
    {
        // Skip writting to Reflection buffer if there's no geometry to reflect off
        needCast = false;
        return;
    }

    MaterialData matData = materialsData[materialID];
    if (matData.specular_specularExp.a <= 200.0f)
    {
        // Skip writting to Reflection buffer if reflection is not perfect mirror reflection
        needCast = false;
        return;
    }

    needCast = true;


    float3 viewPos = CalculateViewPos(UV, depthVal);
    float3 worldPos = CalculateWorldPos(viewPos);

    float3 cameraWorldPos = PerFrame.cameraWorldPos.xyz;
    float3 cameraDirectionWorld = normalize(worldPos - cameraWorldPos);
    float3 worldSpaceNormal = CalculateWorldSpaceNormal(normal);
    float3 reflectionVector = reflect(cameraDirectionWorld, worldSpaceNormal);

    origin = worldPos;
    direction = reflectionVector;
}

[shader("raygeneration")] 
void ReflectionRayGeneration() 
{ 
    float3 origin;
    float3 direction;

    bool needCast;
    CalculateRay(DispatchRaysIndex().xy, origin, direction, needCast);
    if (!needCast)
    {
        return;
    }

    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = direction;
    ray.TMin = 0.001;
    ray.TMax = 10000.0;

    Payload payload = {float4(0, 0, 0, 0)};

    TraceRay(sceneAS, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 1, 0, 0, 0, ray, payload);

    reflectionUAV[DispatchRaysIndex().xy] = payload.color;
}

float InterpolateAttribute(float attributes[3], BuiltInTriangleIntersectionAttributes weights)
{
    return attributes[0] + weights.barycentrics.x * (attributes[1] - attributes[0]) +
           weights.barycentrics.y * (attributes[2] - attributes[0]);
}

float2 InterpolateAttribute(float2 attributes[3], BuiltInTriangleIntersectionAttributes weights)
{
    return attributes[0] + weights.barycentrics.x * (attributes[1] - attributes[0]) +
           weights.barycentrics.y * (attributes[2] - attributes[0]);
}

float3 InterpolateAttribute(float3 attributes[3], BuiltInTriangleIntersectionAttributes weights)
{
    return attributes[0] + 
        weights.barycentrics.x * (attributes[1] - attributes[0]) +
        weights.barycentrics.y * (attributes[2] - attributes[0]);
}

struct Vertex
{
    float2 UV;
    float3 normal;
};

Vertex CombineVertexData(VertexData1 data1, VertexData2 data2) 
{
    Vertex Out = 
    {
        float2(data1.texCoordX, data2.texCoordY),
        data2.normal
    };

    return Out;
}

Vertex InterpolateVertices(Vertex verticies[3], BuiltInTriangleIntersectionAttributes attr)
{
    Vertex Out = (Vertex)0;
    Out.UV = verticies[0].UV + 
             attr.barycentrics.x * (verticies[1].UV - verticies[0].UV) +
             attr.barycentrics.y * (verticies[2].UV - verticies[0].UV);

    Out.normal = verticies[0].normal +
                 attr.barycentrics.x * (verticies[1].normal - verticies[0].normal) +
                 attr.barycentrics.y * (verticies[2].normal - verticies[0].normal);

    return Out;
}

[shader("closesthit")]
void ReflectionClosestHit(inout Payload payload, in BuiltInTriangleIntersectionAttributes attr) {
    uint materialID = InstanceID();
    uint objectIndex = InstanceIndex();
    uint primitiveIndex = PrimitiveIndex();

    uint ojectIndexOffset = rtObjectIndexOffsetBuffer[objectIndex];
    uint indexOffset = ojectIndexOffset + primitiveIndex * 3;

    uint indexes[3] = 
    {
        rtIndexBuffer[indexOffset], 
        rtIndexBuffer[indexOffset + 1],
        rtIndexBuffer[indexOffset + 2]
    };

    VertexData1 vertexData1[] = 
    {
        vertexBuffer1[indexes[0]], 
        vertexBuffer1[indexes[1]], 
        vertexBuffer1[indexes[2]]
    };

    VertexData2 vertexData2[] = 
    {
        vertexBuffer2[indexes[0]], 
        vertexBuffer2[indexes[1]], 
        vertexBuffer2[indexes[2]]
    };

    Vertex verticies[3] =
    {
        CombineVertexData(vertexData1[0], vertexData2[0]),
        CombineVertexData(vertexData1[1], vertexData2[1]),
        CombineVertexData(vertexData1[2], vertexData2[2]),
    };

    Vertex interpolatedVertex = InterpolateVertices(verticies, attr);

    float3 worldPos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();

    float3 albedoVal = SRGBToLinear(sceneTextures[materialID].SampleLevel(anisoSampler, interpolatedVertex.UV, 0).rgb);
    float3 normalVal = normalize(mul(normalize(interpolatedVertex.normal), (float3x3)PerFrame.viewMatrix));
    float3 viewPos = mul(float4(worldPos, 1.0f), PerFrame.viewMatrix).xyz;
    float3 viewDir = mul(float4(WorldRayDirection(), 0.0f), PerFrame.viewMatrix).xyz;

    MaterialData matData = materialsData[materialID];
    payload.color = float4(CalculateLighting(matData, albedoVal, albedoVal, normalVal, viewPos, viewDir), 0.0f);
}

[shader("miss")]
void ReflectionMissShader(inout Payload payload)
{
    float4 skyboxVal = skyBox.SampleLevel(anisoSampler, WorldRayDirection(), 0);
    payload.color = skyboxVal;
}
