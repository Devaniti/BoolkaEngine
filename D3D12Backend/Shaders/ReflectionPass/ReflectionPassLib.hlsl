#include "../Common.hlsli"
#include "../LightingLibrary/Lighting.hlsli"
#include "../CppShared.hlsli"
#include "../Color.hlsli"

// Configure whether we are going to do additional depth samples to select one of two neighbours along each axis
#define BLK_HIGH_QUALITY_RAY_DIFFERENTIALS 1

RayDifferentialPart CalculateRayDifferentialPart(uint2 vpos, float3 origin, float3 direction,
                                                 float depthVal, uint2 offset)
{
    RayDifferentialPart Out = (RayDifferentialPart)0;

#if BLK_HIGH_QUALITY_RAY_DIFFERENTIALS
    float depthValNeighbour[2] = 
    {
        depth.Load(uint3(vpos, 0) - uint3(offset, 0)),
        depth.Load(uint3(vpos, 0) + uint3(offset, 0))
    };

    float depthValDiff[2] =
    { 
        abs(depthVal - depthValNeighbour[0]),
        abs(depthVal - depthValNeighbour[1]),
    };

    float closestNeighbour = 1;
    float depthValUsed;

    if (depthValDiff[0] < depthValDiff[1])
    {
        float closestNeighbour = -1;
        depthValUsed = depthValNeighbour[0];
    }
    else
    {
        float closestNeighbour = 1;
        depthValUsed = depthValNeighbour[1];
    }

    float4 normalValUsed = normal.Load(uint3(vpos, 0) + uint3(closestNeighbour * offset, 0));
#else
    float closestNeighbour = 1;
    float depthValUsed = depth.Load(uint3(vpos, 0) + uint3(offset, 0));
    float4 normalValUsed = normal.Load(uint3(vpos, 0) + uint3(offset, 0));
#endif

    float3 normalUsed = normalValUsed.xyz;

    float2 UVUsed = (float2(vpos) + closestNeighbour * offset) * GetInvBackbufferResolution();
    float3 viewPosUsed = CalculateViewPos(UVUsed, depthValUsed);
    float3 worldPosUsed = CalculateWorldPos(viewPosUsed);

    float3 cameraWorldPos = Frame.cameraWorldPos.xyz;
    float3 cameraDirectionWorldUsed = normalize(worldPosUsed - cameraWorldPos);
    float3 worldSpaceNormalUsed = CalculateWorldSpaceNormal(normalUsed);
    float3 reflectionVectorUsed = reflect(cameraDirectionWorldUsed, worldSpaceNormalUsed);

    Out.dO = (origin - worldPosUsed) * closestNeighbour;
    Out.dD = (direction - reflectionVectorUsed) * closestNeighbour;

    return Out;
}

RayDifferential CalculateRayDifferentials(uint2 vpos, float3 origin, float3 direction,
                                          float depthVal)
{
    RayDifferential Out = 
    {
        CalculateRayDifferentialPart(vpos, origin, direction, depthVal, uint2(1, 0)),
        CalculateRayDifferentialPart(vpos, origin, direction, depthVal, uint2(0, 1))
    };

    return Out;
}

// Calculate rays from camera
// Can be usefull for debugging
//inline void CalculateRayCamera(uint2 vpos, out float3 origin, out float3 direction)
//{
//    float2 UV = vpos * GetInvBackbufferResolution();
//    float4 viewProjPos = float4(UV.x * 2.0f - 1.0f, UV.y * -2.0f + 1.0f, 0.0f, 1.0f);
//
//    float4 worldPos = mul(viewProjPos, Frame.invViewProjMatrix);
//
//    worldPos.xyz /= worldPos.w;
//    origin = Frame.cameraWorldPos.xyz;
//    direction = normalize(worldPos.xyz - origin);
//}
//
//inline void CalculateRay(uint2 vpos, out float3 origin, out float3 direction,
//                         out RayDifferential rayDifferential, out bool needCast)
//{
//    needCast = true;
//
//    CalculateRayCamera(vpos, origin, direction);
//
//    float3 originX, directionX;
//    CalculateRayCamera(vpos + uint2(1, 0), originX, directionX);
//    rayDifferential.dx.dO = originX - origin;
//    rayDifferential.dx.dD = directionX - direction;
//
//    float3 originY, directionY;
//    CalculateRayCamera(vpos + uint2(0, 1), originY, directionY);
//    rayDifferential.dy.dO = originY - origin;
//    rayDifferential.dy.dD = directionY - direction;
//}

inline void CalculateRay(uint2 vpos, out float3 origin, out float3 direction,
                         out RayDifferential rayDifferential, out bool needCast)
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

    float3 cameraWorldPos = Frame.cameraWorldPos.xyz;
    float3 cameraDirectionWorld = normalize(worldPos - cameraWorldPos);
    float3 worldSpaceNormal = CalculateWorldSpaceNormal(normal);
    float3 reflectionVector = reflect(cameraDirectionWorld, worldSpaceNormal);

    origin = worldPos;
    direction = reflectionVector;

    rayDifferential = CalculateRayDifferentials(vpos, origin, direction, depthVal);
}

[shader("raygeneration")] 
void ReflectionRayGeneration() 
{ 
    float3 origin;
    float3 direction;
    RayDifferential rayDifferential = (RayDifferential)0;

    bool needCast;
    CalculateRay(DispatchRaysIndex().xy, origin, direction, rayDifferential, needCast);
    if (!needCast)
    {
        return;
    }

    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = direction;
    ray.TMin = 0.01f;
    ray.TMax = 10000.0f;

    ReflectionPayload payload = 
    {
        float4(0, 0, 0, 0),
        rayDifferential
    };

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
    float3 position;
    float3 normal;
    float2 UV;
};

Vertex CombineVertexData(VertexData1 data1, VertexData2 data2) 
{
    Vertex Out = 
    {
        data1.position,
        data2.normal,
        float2(data1.texCoordX, data2.texCoordY)
    };

    return Out;
}

Vertex InterpolateVertices(Vertex verticies[3], BuiltInTriangleIntersectionAttributes attr)
{
    Vertex Out = (Vertex)0;

    Out.position = verticies[0].position +
                   attr.barycentrics.x * (verticies[1].position - verticies[0].position) +
                   attr.barycentrics.y * (verticies[2].position - verticies[0].position);

    Out.UV = verticies[0].UV + 
             attr.barycentrics.x * (verticies[1].UV - verticies[0].UV) +
             attr.barycentrics.y * (verticies[2].UV - verticies[0].UV);

    Out.normal = verticies[0].normal +
                 attr.barycentrics.x * (verticies[1].normal - verticies[0].normal) +
                 attr.barycentrics.y * (verticies[2].normal - verticies[0].normal);

    return Out;
}

void CalculateDDXDDY(Vertex vertices[3], RayDifferential rayDifferential, out float2 ddxRes, out float2 ddyRes)
{
    float3 e1 = vertices[1].position - vertices[0].position;
    float3 e2 = vertices[2].position - vertices[0].position;
    float3 t = RayTCurrent();
    float3 d = WorldRayDirection();

    float3 cu = cross(e2, d);
    float3 cv = cross(d, e1);
    float3 q = rayDifferential.dx.dO + t * rayDifferential.dx.dD;
    float3 r = rayDifferential.dy.dO + t * rayDifferential.dy.dD;

    float k = dot(cross(e1, e2), d);

    float dudx = dot(1.0f / k * cu, q);
    float dudy = dot(1.0f / k * cu, r);
    float dvdx = dot(1.0f / k * cv, q);
    float dvdy = dot(1.0f / k * cv, r);

    float2 g1 = vertices[1].UV - vertices[0].UV;
    float2 g2 = vertices[2].UV - vertices[0].UV;

    ddxRes = dudx * g1 + dvdx * g2;
    ddyRes = dudy * g1 + dvdy * g2;
}

[shader("closesthit")]
void ReflectionClosestHit(inout ReflectionPayload payload, in BuiltInTriangleIntersectionAttributes attr) {
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

    Vertex vertices[3] =
    {
        CombineVertexData(vertexData1[0], vertexData2[0]),
        CombineVertexData(vertexData1[1], vertexData2[1]),
        CombineVertexData(vertexData1[2], vertexData2[2]),
    };

    float2 ddxRes, ddyRes;
    CalculateDDXDDY(vertices, payload.rayDifferential, ddxRes, ddyRes);

    Vertex interpolatedVertex = InterpolateVertices(vertices, attr);

    float3 worldPos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    
    float3 albedoVal = SRGBToLinear(sceneTextures[materialID].SampleGrad(anisoSampler, interpolatedVertex.UV, ddxRes, ddyRes).rgb);
    float3 normalVal = normalize(mul(normalize(interpolatedVertex.normal), (float3x3)Frame.viewMatrix));
    float3 viewPos = mul(float4(worldPos, 1.0f), Frame.viewMatrix).xyz;
    float3 viewDir = mul(float4(WorldRayDirection(), 0.0f), Frame.viewMatrix).xyz;

    MaterialData matData = materialsData[materialID];
    payload.color = float4(CalculateLighting(matData, albedoVal, albedoVal, normalVal, viewPos, viewDir), 0.0f);
}

[shader("miss")]
void ReflectionMissShader(inout ReflectionPayload payload)
{
    float4 skyboxVal = skyBox.SampleLevel(anisoSampler, WorldRayDirection(), 0);
    payload.color = skyboxVal;
}
