#include "../Common.hlsli"
#include "../LightingLibrary/Lighting.hlsli"
#include "../CppShared.hlsli"
#include "../Color.hlsli"

// Configure whether we are going to do additional depth samples to select one of two neighbours along each axis
#define BLK_HIGH_QUALITY_RAY_DIFFERENTIALS 1

RayDifferentialPart CalculateRayDifferentialGBPart(uint2 vpos, float3 origin, float3 direction,
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

RayDifferential CalculateRayDifferentialsGB(uint2 vpos, float3 origin, float3 direction,
                                          float depthVal)
{
    RayDifferential Out = 
    {
        CalculateRayDifferentialGBPart(vpos, origin, direction, depthVal, uint2(1, 0)),
        CalculateRayDifferentialGBPart(vpos, origin, direction, depthVal, uint2(0, 1))
    };

    return Out;
}

inline void CalculateRayGB(uint2 vpos, out float3 origin, out float3 direction,
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

    rayDifferential = CalculateRayDifferentialsGB(vpos, origin, direction, depthVal);
}

[shader("raygeneration")] 
void ReflectionRayGeneration() 
{ 
    float3 origin;
    float3 direction;
    RayDifferential rayDifferential = (RayDifferential)0;

    bool needCast;
    CalculateRayGB(DispatchRaysIndex().xy, origin, direction, rayDifferential, needCast);
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
        float3(0, 0, 0),
        0,
        rayDifferential
    };

    TraceRay(sceneAS, RAY_FLAG_NONE, 1, 0, 0, 0, ray, payload);

    reflectionUAV[DispatchRaysIndex().xy] = float4(payload.light, 0.0f);
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

// Interpolates vertex, calculated ddx/ddy, updates RayDifferential for next reflection bounce
// RayDifferentials reference here http://www.realtimerendering.com/raytracinggems/unofficial_RayTracingGems_v1.9.pdf chapter 20
void CalculateRTHit(in const Vertex verticies[3],
                       in const BuiltInTriangleIntersectionAttributes attr,
                       inout RayDifferential rayDifferential, out Vertex outVertex,
                       out float2 ddxRes, out float2 ddyRes)
{
    // Calculating differences for barycentric coordinate interpolation
    float3 e1 = verticies[1].position - verticies[0].position;
    float3 e2 = verticies[2].position - verticies[0].position;

    float2 g1 = verticies[1].UV - verticies[0].UV;
    float2 g2 = verticies[2].UV - verticies[0].UV;

    float3 n1 = verticies[1].normal - verticies[0].normal;
    float3 n2 = verticies[2].normal - verticies[0].normal;

    // Interpolating hit triangle verticies
    outVertex.position = verticies[0].position + attr.barycentrics.x * e1 + attr.barycentrics.y * e2;
    outVertex.UV = verticies[0].UV + attr.barycentrics.x * g1 + attr.barycentrics.y * g2;
    outVertex.normal = verticies[0].normal + attr.barycentrics.x * n1 + attr.barycentrics.y * n2;

    // Calculating ddx/ddy for texture sampling
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

    ddxRes = dudx * g1 + dvdx * g2;
    ddyRes = dudy * g1 + dvdy * g2;

    // Updating ray differentials for recursive reflection ray trace
    rayDifferential.dx.dO = dudx * e1 + dvdx * e2;
    rayDifferential.dy.dO = dudy * e1 + dvdy * e2;

    // This part is not documented in raytracing gems
    // First ray direction derivative gets reflected
    // That would have been enough if hit surface was perfectly flat
    rayDifferential.dx.dD = reflect(rayDifferential.dx.dD, outVertex.normal);
    rayDifferential.dy.dD = reflect(rayDifferential.dy.dD, outVertex.normal);

    // Calculating differential of normals same way we calculated differential of texcoords
    // both will be 0 in case of perfectly flat surface
    float3 dndx = dudx * n1 + dvdx * n2;
    float3 dndy = dudy * n1 + dvdy * n2;

    // Updated ray direction differential approximately equal to
    // sum of ray differential for flat surface case plus double normal differential
    // this is because if you rotate surface by N degrees your reflection ray will rotate by 2*N degrees
    rayDifferential.dx.dD += 2 * dndx;
    rayDifferential.dy.dD += 2 * dndy;
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

    Vertex interpolatedVertex = (Vertex)0;
    float2 ddxRes, ddyRes;
    CalculateRTHit(vertices, attr, payload.rayDifferential, interpolatedVertex, ddxRes, ddyRes);

    float3 worldPos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    
    float3 albedoVal = SRGBToLinear(sceneTextures[materialID].SampleGrad(anisoSampler, interpolatedVertex.UV, ddxRes, ddyRes).rgb);
    float3 normalVal = normalize(mul(normalize(interpolatedVertex.normal), (float3x3)Frame.viewMatrix));
    float3 viewPos = mul(float4(worldPos, 1.0f), Frame.viewMatrix).xyz;
    float3 viewDir = mul(float4(WorldRayDirection(), 0.0f), Frame.viewMatrix).xyz;

    MaterialData matData = materialsData[materialID];
    payload.light += CalculateLighting(matData, albedoVal, albedoVal, normalVal, viewPos, viewDir);

    if (payload.recursionDepth < BLK_REFLECTION_RT_MAX_RECURSION_DEPTH - 1)
    {
        if (matData.specular_specularExp.a > 200.0f)
        {
            payload.recursionDepth++;

            RayDesc ray;
            ray.Origin = worldPos;
            ray.Direction = reflect(WorldRayDirection(), normalize(interpolatedVertex.normal));
            ray.TMin = 0.01f;
            ray.TMax = 10000.0f;

            TraceRay(sceneAS, RAY_FLAG_NONE, 1, 0, 0, 0, ray, payload);
        }
    }
}

[shader("miss")]
void ReflectionMissShader(inout ReflectionPayload payload)
{
    float3 skyboxVal = skyBox.SampleLevel(anisoSampler, WorldRayDirection(), 0).xyz;
    payload.light += skyboxVal;
}
