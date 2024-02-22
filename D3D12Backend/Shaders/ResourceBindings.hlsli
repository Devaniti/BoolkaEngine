#ifndef __RESOURCE_BINDINGS_HLSLI__
#define __RESOURCE_BINDINGS_HLSLI__

// Static samplers
SamplerState pointSampler : register(s0);
SamplerState linearSingleMipSampler : register(s1);
SamplerState linearSampler : register(s2);
SamplerState anisoSampler : register(s3);
SamplerComparisonState shadowSampler : register(s4);

// Root CBs
ConstantBuffer<FrameConstantBuffer> Frame : register(b0);

// Descriptor table CBs
ConstantBuffer<FrameConstantBuffer> FrameNonRoot : register(b0, space1);
ConstantBuffer<LightingDataConstantBuffer> LightingNonRoot : register(b1, space1);
ConstantBuffer<CullingDataConstantBuffer> GPUCulling : register(b2, space1);

// UAVs
RWTexture2D<float4> raytraceUAV : register(u0);
RWStructuredBuffer<CullingCommandSignature> gpuCullingCommandUAV : register(u1);
RWStructuredBuffer<uint> gpuCullingMeshletIndicesUAV : register(u2);
RWTexture1D<float4> tonemappingLUTUAV : register(u3);
RWStructuredBuffer<uint> profileMetrics : register(u4);
RWStructuredBuffer<uint> debugMarkers : register(u5);

// Pipeline resources
Texture2D<float4> albedo : register(t0);
Texture2D<float4> normal : register(t1);
Texture2D<float4> raytraceResults : register(t2);
Texture2D<float> depth : register(t3);
Texture2D<float4> lightBuffer : register(t4);
StructuredBuffer<uint> gpuCullingMeshletIndices : register(t5);
TextureCube<float> shadowMapCube[4] : register(t6);
Texture2D<float> shadowMapSun : register(t10);
Texture1D<float> tonemappingLUT : register(t11);

// Meshlet data
StructuredBuffer<VertexData1> vertexBuffer1 : register(t0, space1);
StructuredBuffer<VertexData2> vertexBuffer2 : register(t1, space1);
StructuredBuffer<uint> vertexIndirectionBuffer : register(t2, space1);
StructuredBuffer<uint> indexBuffer : register(t3, space1);
StructuredBuffer<MeshletData> meshletBuffer : register(t4, space1);
StructuredBuffer<MeshletCullData> meshletCullBuffer : register(t5, space1);
StructuredBuffer<ObjectData> objectBuffer : register(t6, space1);
StructuredBuffer<MaterialData> materialsData : register(t7, space1);

// RT data
StructuredBuffer<uint> rtIndexBuffer : register(t0, space2);
StructuredBuffer<uint> rtObjectIndexOffsetBuffer : register(t1, space2);
RaytracingAccelerationStructure sceneAS : register(t2, space2);

// Skybox
TextureCube<float4> skyBox : register(t0, space3);

// Scene textures
Texture2D<float4> sceneTextures[] : register(t0, space4);

#endif
