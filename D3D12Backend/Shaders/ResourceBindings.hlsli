#ifndef __RESOURCE_BINDINGS_HLSLI__
#define __RESOURCE_BINDINGS_HLSLI__

SamplerState pointSampler : register(s0);
SamplerState linearSingleMipSampler : register(s1);
SamplerState linearSampler : register(s2);
SamplerState anisoSampler : register(s3);
SamplerComparisonState shadowSampler : register(s4);

RWTexture2D<float4> reflectionUAV : register(u0);

Texture2D<float4> albedo : register(t0);
Texture2D<float4> normal : register(t1);
Texture2D<float4> reflections : register(t2);
Texture2D<float> depth : register(t3);
Texture2D<float4> lightBuffer : register(t4);
TextureCube<float> shadowMapCube[4] : register(t5);
Texture2D<float> shadowMapSun : register(t9);

Texture2D<float4> sceneTextures[] : register(t0, space1);

StructuredBuffer<VertexData1> vertexBuffer1 : register(t0, space2);
StructuredBuffer<VertexData2> vertexBuffer2 : register(t1, space2);

StructuredBuffer<MaterialData> materialsData : register(t6, space2);

StructuredBuffer<uint> rtIndexBuffer : register(t7, space2);
StructuredBuffer<uint> rtObjectIndexOffsetBuffer : register(t8, space2);
RaytracingAccelerationStructure sceneAS : register(t9, space2);

TextureCube<float4> skyBox : register(t10, space2);  

#endif
