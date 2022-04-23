#include "../Common.hlsli"
#include "../LightingLibrary/Lighting.hlsli"
#include "../CppShared.hlsli"
#include "../Color.hlsli"

// Configure whether we are going to do additional depth samples to select one of two neighbours along each axis
#define BLK_HIGH_QUALITY_RAY_DIFFERENTIALS 1

#define BLK_REFLECTION_TMIN 0.5f
#define BLK_REFLECTION_TMAX 10000.0f
#define BLK_REFRACTION_TMIN 0.5f
#define BLK_REFRACTION_TMAX 10000.0f

bool IsPerfectMirror(in const MaterialData matData)
{
    return matData.specularExp > 200.0f;
}

bool IsTransparent(in const MaterialData matData)
{
    return matData.transparency < 1.0f;
}

struct GBufferData
{
    float depth;
    float3 normal;
    uint materialID;
};

GBufferData LoadGBuffer()
{
    GBufferData result;
    uint2 vpos = DispatchRaysIndex().xy;

    result.depth = depth.Load(uint3(vpos, 0));
    float4 normalVal = normal.Load(uint3(vpos, 0));
    result.normal = normalVal.xyz;
    result.materialID = uint(normalVal.w);

    return result;
}

MaterialData LoadMaterialData(const in GBufferData gbufferData)
{
    return materialsData[gbufferData.materialID];
}

RayDifferentialPart CalculateReflectionRayDifferentialGBPart(in const GBufferData gbufferData,
                                                             float3 origin, float3 direction,
                                                             uint2 offset)
{
    uint2 vpos = DispatchRaysIndex().xy;

    RayDifferentialPart Out = (RayDifferentialPart)0;

#if BLK_HIGH_QUALITY_RAY_DIFFERENTIALS
    float depthValNeighbour[2] = 
    {
        depth.Load(uint3(vpos, 0) - uint3(offset, 0)),
        depth.Load(uint3(vpos, 0) + uint3(offset, 0))
    };

    float depthValDiff[2] =
    { 
        abs(gbufferData.depth - depthValNeighbour[0]),
        abs(gbufferData.depth - depthValNeighbour[1]),
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
    // TODO find a way to use wave intrinsics to optimize this codepath
    float closestNeighbour = 1;
    float depthValUsed = depth.Load(uint3(vpos, 0) + uint3(offset, 0));
    float4 normalValUsed = normal.Load(uint3(vpos, 0) + uint3(offset, 0));
#endif

    float3 normalUsed = normalValUsed.xyz;

    float2 UVUsed = (float2(vpos) + closestNeighbour * offset) * Frame.invBackBufferResolution;
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

RayDifferential CalculateReflectionRayDifferentialsGB(in const GBufferData gbufferData,
                                                      float3 origin, float3 direction)
{
    RayDifferential Out = 
    {
        CalculateReflectionRayDifferentialGBPart(gbufferData, origin, direction, uint2(1, 0)),
        CalculateReflectionRayDifferentialGBPart(gbufferData, origin, direction, uint2(0, 1))
    };

    return Out;
}

RayDifferential CalculateRefractionRayDifferentialsGB(in const GBufferData gbufferData,
                                                      float3 origin, float3 direction)
{
    float3 cameraWorldPos = Frame.cameraWorldPos.xyz;
    float distToCamera = length(origin - cameraWorldPos);

    uint2 vpos = DispatchRaysIndex().xy;
    float2 UV = vpos * Frame.invBackBufferResolution;
    float2 c = 2 * UV - 1;
    float3 rd = Frame.eyeRayCoeficients[3].xyz;
    float3 ud = Frame.eyeRayCoeficients[4].xyz;

    RayDifferential Out = {
        {rd * distToCamera, rd},
        {ud * distToCamera, ud},
    };

    return Out;
}

//TODO Unify Refraction and Reflection ray calculation, they share similar computations.
inline void CalculateReflectionRayGB(in const GBufferData gbufferData,
                                     in const MaterialData matData, out float3 origin,
                                     out float3 direction, out float3 attenuation,
                                     out RayDifferential rayDifferential)
{
    uint2 vpos = DispatchRaysIndex().xy;
    float2 UV = vpos * Frame.invBackBufferResolution;
    float3 viewPos = CalculateViewPos(UV, gbufferData.depth);
    float3 worldPos = CalculateWorldPos(viewPos);

    float3 cameraWorldPos = Frame.cameraWorldPos.xyz;
    float3 cameraDirectionWorld = normalize(worldPos - cameraWorldPos);
    float3 worldSpaceNormal = CalculateWorldSpaceNormal(gbufferData.normal);
    float3 reflectionVector = reflect(cameraDirectionWorld, worldSpaceNormal);

    float3 fresnel = Fresnel(cameraDirectionWorld, worldSpaceNormal, matData.specular);

    attenuation = fresnel;
    origin = worldPos;
    direction = reflectionVector;

    rayDifferential =
        CalculateReflectionRayDifferentialsGB(gbufferData, origin, direction);
}

float3 GetCameraRay()
{
    uint2 vpos = DispatchRaysIndex().xy;
    float2 UV = (vpos + 0.5f) * Frame.invBackBufferResolution;

    float2 c = 2 * UV - 1;

    float3 r = Frame.eyeRayCoeficients[0].xyz;
    float3 u = Frame.eyeRayCoeficients[1].xyz;
    float3 v = Frame.eyeRayCoeficients[2].xyz;

    float3 direction = c.x * r + c.y * u + v;
    return normalize(direction);
}

inline void CalculateRefractionRayGB(in const GBufferData gbufferData,
                                     in const MaterialData matData, out float3 origin,
                                     out float3 direction, out float3 attenuation,
                                     out RayDifferential rayDifferential)
{
    uint2 vpos = DispatchRaysIndex().xy;
    float2 UV = vpos * Frame.invBackBufferResolution;
    float3 viewPos = CalculateViewPos(UV, gbufferData.depth);
    float3 worldPos = CalculateWorldPos(viewPos);
    float3 worldSpaceNormal = CalculateWorldSpaceNormal(gbufferData.normal);
    
    float3 fresnel = Fresnel(GetCameraRay(), worldSpaceNormal, matData.specular);

    attenuation = (1.0 - fresnel);

    origin = worldPos;
    direction = refract(GetCameraRay(), worldSpaceNormal, 1.0/matData.indexOfRefraction);
    rayDifferential = CalculateRefractionRayDifferentialsGB(gbufferData, origin, direction);

    return;
}

void ReflectionRayGeneration(in const GBufferData gbufferData,
                             in const MaterialData matData, inout float3 light)
{ 
    float3 origin;
    float3 direction;
    float3 attenuation;
    RayDifferential rayDifferential = (RayDifferential)0;

    CalculateReflectionRayGB(gbufferData, matData, origin, direction, attenuation, rayDifferential);

    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = direction;
    ray.TMin = BLK_REFLECTION_TMIN;
    ray.TMax = BLK_REFLECTION_TMAX;

    RaytracePayload payload = {float3(0, 0, 0), 0, float3(1, 1, 1), matData.indexOfRefraction, ray.Origin, 0, rayDifferential};
    payload.color = attenuation;

    TraceRay(sceneAS, RAY_FLAG_NONE, 1, 0, 0, 0, ray, payload);
    light += payload.light*payload.color;
}

// No real support for refraction yet
// We don't change ray direction yet
// TODO add refractive objects to the scene and implement propper refraction
// That would require manual edit of the scene
void RefractionRayGeneration(in const GBufferData gbufferData, in const MaterialData matData,
                             inout float3 light)
{
    float3 origin;
    float3 direction;
    float3 attenuation;
    RayDifferential rayDifferential = (RayDifferential)0;

    CalculateRefractionRayGB(gbufferData, matData, origin, direction, attenuation, rayDifferential);

    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = direction;
    ray.TMin = BLK_REFRACTION_TMIN;
    ray.TMax = BLK_REFRACTION_TMAX;

    RaytracePayload payload = {float3(0, 0, 0), 0, float3(1, 1, 1), 1.0/matData.indexOfRefraction, ray.Origin, 0, rayDifferential};
    payload.color = attenuation;

    TraceRay(sceneAS, RAY_FLAG_NONE, 1, 0, 0, 0, ray, payload);
    light += payload.light*payload.color;
}

[shader("raygeneration")] 
void RayGeneration() 
{
    GBufferData gbufferData = LoadGBuffer();
    MaterialData matData = LoadMaterialData(gbufferData);
        
    if (gbufferData.depth == 1.0f) // Far plane
    {
        return;
    }

    if (!IsPerfectMirror(matData))
    {
        return;
    }

    float3 light = 0.0f;
    ReflectionRayGeneration(gbufferData, matData, light);
    RefractionRayGeneration(gbufferData, matData, light);

    raytraceUAV[DispatchRaysIndex().xy] = float4(light, 0.0f);
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

void GetVertices(out Vertex outVertices[3])
{
    uint objectIndex = InstanceIndex();
    uint primitiveIndex = PrimitiveIndex();

    uint ojectIndexOffset = rtObjectIndexOffsetBuffer[objectIndex];
    uint indexOffset = ojectIndexOffset + primitiveIndex * 3;

    uint indexes[3] = {rtIndexBuffer[indexOffset], rtIndexBuffer[indexOffset + 1],
                       rtIndexBuffer[indexOffset + 2]};

    VertexData1 vertexData1[] = {vertexBuffer1[indexes[0]], vertexBuffer1[indexes[1]],
                                 vertexBuffer1[indexes[2]]};

    VertexData2 vertexData2[] = {vertexBuffer2[indexes[0]], vertexBuffer2[indexes[1]],
                                 vertexBuffer2[indexes[2]]};

    outVertices[0] = CombineVertexData(vertexData1[0], vertexData2[0]);
    outVertices[1] = CombineVertexData(vertexData1[1], vertexData2[1]);
    outVertices[2] = CombineVertexData(vertexData1[2], vertexData2[2]);
}

// Interpolates vertex, calculated ddx/ddy, updates RayDifferential's dO for recursive raytracing
// RayDifferentials reference here http://www.realtimerendering.com/raytracinggems/unofficial_RayTracingGems_v1.9.pdf chapter 20
void CalculateRTHit(in const BuiltInTriangleIntersectionAttributes attr,
                    inout RayDifferential rayDifferential, out Vertex outVertex,
                    out float2 ddxRes, out float2 ddyRes, out float3 dndx, out float3 dndy)
{
    Vertex verticies[3];
    GetVertices(verticies);

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

    // Updating ray differentials for recursive ray trace
    // Same for both reflection and refraction, only dD will differ for that cases
    rayDifferential.dx.dO = dudx * e1 + dvdx * e2;
    rayDifferential.dy.dO = dudy * e1 + dvdy * e2;

    // Calculating differential of normals same way we calculated differential of texcoords
    // Needed to get better aprixomation for RayDifferential's dD
    // both will be 0 in case of perfectly flat surface
    dndx = dudx * n1 + dvdx * n2;
    dndy = dudy * n1 + dvdy * n2;
}

// Updates dD for refraction bounce
// This part is not documented in raytracing gems
void UpdateRayDifferentialsReflection(inout RayDifferential rayDifferential, float3 normal,
                                      float3 dndx, float3 dndy)
{
    // First ray direction derivative gets reflected
    // That would have been enough if hit surface was perfectly flat
    rayDifferential.dx.dD = reflect(rayDifferential.dx.dD, normal);
    rayDifferential.dy.dD = reflect(rayDifferential.dy.dD, normal);

    // Updated ray direction differential approximately equal to
    // sum of ray differential for flat surface case plus double normal differential
    // this is because if you rotate surface by N degrees your reflection ray will rotate by 2*N
    // degrees
    rayDifferential.dx.dD += 2 * dndx;
    rayDifferential.dy.dD += 2 * dndy;
}

// Updates dD for reflection bounce
void UpdateRayDifferentialsRefraction(inout float3 dDdx, inout float3 dDdy, float3 normal, float3 ior,
                                      float3 dndx, float3 dndy)
{
    // No refraction at the moment
    // Default scene doesn't have refractive materials
    // Since there's no refraction old value is good for us
}

[shader("closesthit")]
void ClosestHit(inout RaytracePayload payload, in BuiltInTriangleIntersectionAttributes attr) {
    uint materialID = InstanceID();

    Vertex interpolatedVertex = (Vertex)0;
    float2 ddxRes, ddyRes;
    float3 dndx, dndy;
    CalculateRTHit(attr, payload.rayDifferential, interpolatedVertex, ddxRes,
                   ddyRes, dndx, dndy);

    const float3 worldPos = interpolatedVertex.position;
    
    float3 albedoVal = SRGBToLinear(sceneTextures[materialID].SampleGrad(anisoSampler, interpolatedVertex.UV, ddxRes, ddyRes).rgb);
    float3 normalVal = normalize(mul(normalize(interpolatedVertex.normal), (float3x3)Frame.viewMatrix));
    float3 viewPos = mul(float4(worldPos, 1.0f), Frame.viewMatrix).xyz;
    float3 viewDir = mul(float4(WorldRayDirection(), 0.0f), Frame.viewMatrix).xyz;

    if(payload.previousIOR < 1.0)//meaning ray is moving inside
    {
        //TODO add this to material
        float depth = distance(worldPos, payload.previousPos)/10.0;
        //absorption
        payload.color *= exp(-depth * (1.0 - float3(0.1, 0.3, 0.9)));
    }

    MaterialData matData = materialsData[materialID];
    payload.light += CalculateLighting(matData, albedoVal, albedoVal, normalVal, viewPos, viewDir) *
                     payload.color;

    float3 originalColor = payload.color;
    float3 originaldDdx = payload.rayDifferential.dx.dD;
    float3 originaldDdy = payload.rayDifferential.dy.dD;
    uint originalRecursionDepth = payload.recursionDepth;



    if (payload.recursionDepth < BLK_RT_MAX_RECURSION_DEPTH - 1)
    {
        float3 rayDir = WorldRayDirection();
        float3 normal = normalize(interpolatedVertex.normal);

        bool isUnderwater = dot(normal, rayDir) > 0.0;

        // Refraction
      
        payload.recursionDepth = originalRecursionDepth + 1;
        payload.previousPos = worldPos;

        RayDesc ray;
        ray.Origin = worldPos;

        if(isUnderwater)
        {
            float3 fresnel = Fresnel(rayDir, -normal, matData.specular);
            payload.color = originalColor;// *fresnel;
            payload.previousIOR = matData.indexOfRefraction;
            ray.Direction = refract(rayDir, -normal, matData.indexOfRefraction);
            if(length(ray.Direction) < 0.5) //full internal reflection
            {
                payload.previousIOR = 1.0/matData.indexOfRefraction;
                ray.Direction = reflect(rayDir, -normal);
                payload.color = originalColor;
            }
        }
        else
        {
            float3 fresnel = Fresnel(rayDir, normal, matData.specular);
            payload.color = originalColor;// * fresnel;
            payload.previousIOR = 1.0/matData.indexOfRefraction;
            ray.Direction = refract(rayDir, normal, 1.0/matData.indexOfRefraction);
        }

        ray.TMin = BLK_REFRACTION_TMIN;
        ray.TMax = BLK_REFRACTION_TMAX;

        TraceRay(sceneAS, RAY_FLAG_NONE, 1, 0, 0, 0, ray, payload);
        
        //TODO figure out this for all cases
        /*// Reflection
        if (IsPerfectMirror(matData))
        {
            payload.recursionDepth = originalRecursionDepth + 1;
            payload.color = originalColor * matData.transparency;
            payload.rayDifferential.dx.dD = originaldDdx;
            payload.rayDifferential.dy.dD = originaldDdy;
            UpdateRayDifferentialsReflection(payload.rayDifferential, normalVal, dndx, dndy);

            RayDesc ray;
            ray.Origin = worldPos;
            ray.Direction =  reflect(WorldRayDirection(), normalize(interpolatedVertex.normal));
            ray.TMin = BLK_REFRACTION_TMIN;
            ray.TMax = BLK_REFRACTION_TMAX;

            TraceRay(sceneAS, RAY_FLAG_NONE, 1, 0, 0, 0, ray, payload);
        }*/
    }
}

[shader("miss")]
void MissShader(inout RaytracePayload payload)
{
    float3 skyboxVal = skyBox.SampleLevel(anisoSampler, WorldRayDirection(), 0).xyz;
    //TODO Implement post process shader with exposure
    payload.light += 3.0*skyboxVal;
}
