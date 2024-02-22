#ifndef __CPP_SHARED_HLSLI__
#define __CPP_SHARED_HLSLI__

#ifdef __cplusplus

using float4x4 = Matrix4x4;
using float2 = Vector2;
using float3 = Vector3;
using float4 = Vector4;
using uint2 = Vector2u;
using uint3 = Vector3u;
using uint4 = Vector4u;
// Frustum and AABB are types in Boolka namespace

#else

struct Frustum
{
    float4 planes[6];
};

struct AABB
{
    float4 min;
    float4 max;
};

#endif

#define BLK_MIN_WAVE_SIZE 4

#define BLK_RENDER_VIEW_COUNT 26
#define BLK_MAX_SCENE_TEXTURE_COUNT 512
#define BLK_MAX_OBJECT_COUNT 2048
#define BLK_MAX_MESHLETS 262144
#define BLK_AS_GROUP_SIZE 32
#define BLM_MAX_AS_GROUPS (BLK_MAX_MESHLETS / BLK_AS_GROUP_SIZE)

#define BLK_RT_MAX_RECURSION_DEPTH 4

// Only used for performance/quality testing
// Undefining this doesn't prevent resource creation
#define BLK_TONEMAPPING_USE_LUT
#define BLK_TONEMAPPING_LUT_RESOLUTION 32
#define BLK_TONEMAPPING_LUT_GENERATION_GROUP_SIZE 32

#define BLK_DEBUG_DATA_ELEMENT_COUNT 4096

#define BLK_PROFILING_DATA_GPU_CULLING_OFFSET 0
#define BLK_PROFILING_DATA_GPU_CULLING_VALUES_PER_VIEW 2
#define BLK_PROFILING_DATA_GPU_CULLING_SIZE (BLK_PROFILING_DATA_GPU_CULLING_VALUES_PER_VIEW * BLK_RENDER_VIEW_COUNT)
#define BLK_PROFILING_DATA_ELEMENT_COUNT BLK_PROFILING_DATA_GPU_CULLING_SIZE

struct FrameConstantBuffer
{
    float4x4 viewProjMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 invViewProjMatrix;
    float4x4 invViewMatrix;
    float4x4 invProjMatrix;
    Frustum mainViewFrustum;
    // Coeficients explained in Raytracing Gems 1 in 20.3.2.1 EYE RAY SETUP
    // 0-2 - r u v
    // 3-4 - rd ud
    float4 eyeRayCoeficients[5];
    float4 cameraWorldPos;
    float2 backbufferResolution;
    float2 invBackBufferResolution;
};

struct LightData
{
    float3 viewPos;
    float nearZ;
    float3 color;
    float farZ;
};

struct SunData
{
    float4 lightDirVS;
    float4 color;
    float4x4 viewToShadow;
};

struct LightingDataConstantBuffer
{
    SunData sun;
    uint4 lightCount;
    LightData lights[4];
};

struct CullingDataConstantBuffer
{
    Frustum views[BLK_RENDER_VIEW_COUNT];
    float4x4 viewProjMatrix[BLK_RENDER_VIEW_COUNT];
    float4 cameraPos[BLK_RENDER_VIEW_COUNT];
    uint4 objectCount;
};

struct MaterialData
{
    float3 diffuse;
    float transparency;
    float3 specular;
    float specularExp;
    float indexOfRefraction;
    float3 padding;
};

struct RayDifferentialPart
{
    float3 dO;
    float3 dD;
};

struct RayDifferential
{
    RayDifferentialPart dx;
    RayDifferentialPart dy;
};

struct RaytracePayload
{
    float3 light;
    uint recursionDepth;
    float3 color;
    uint unused;
    RayDifferential rayDifferential;
};

struct VertexData1
{
    float3 position;
    float texCoordX;
};

struct VertexData2
{
    float3 normal;
    float texCoordY;
};

struct MeshletData
{
    uint16_t MaterialID;
    uint16_t VertCount;
    uint VertOffset;
    uint16_t unused;
    uint16_t PrimCount;
    uint PrimOffset;
};

struct MeshletCullData
{
    float4 BoundingSphere;
    uint NormalCone;
    float ApexOffset;
    float2 unused;
};

struct ObjectData
{
    AABB boundingBox;

    uint meshletOffset;
    uint meshletCount;
    uint2 unused;
};

struct CullingCommandSignature
{
    uint3 amplificationShaderGroups;
};

#endif
