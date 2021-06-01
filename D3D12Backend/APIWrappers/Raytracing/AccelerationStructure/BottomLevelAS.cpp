#include "stdafx.h"

#include "BottomLevelAS.h"

#include "APIWrappers/Device.h"

namespace Boolka
{
    void BottomLevelAS::GetSizes(Device& device, UINT vertexCount, UINT vertexStride,
                                 UINT indexCount, UINT64& outScratchSize, UINT64& outBLASSize)
    {
        // ID3D12Device5::GetRaytracingAccelerationStructurePrebuildInfo may check which pointers
        // are NULL when calculating required size, but it's not allowed to actually use that
        // pointers. Using dummy not NULL pointer we can specify that there will be valid non NULL
        // pointer during build.
        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device5-getraytracingaccelerationstructureprebuildinfo
        D3D12_GPU_VIRTUAL_ADDRESS dummyNotNullPointer = 0x1;

        D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
        geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        geometryDesc.Triangles.IndexBuffer = dummyNotNullPointer;
        geometryDesc.Triangles.IndexCount = indexCount;
        geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
        geometryDesc.Triangles.Transform3x4 = 0;
        geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        geometryDesc.Triangles.VertexCount = vertexCount;
        geometryDesc.Triangles.VertexBuffer.StartAddress = dummyNotNullPointer;
        geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexStride;

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputsDesc = {};
        inputsDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        inputsDesc.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE |
                           D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
        inputsDesc.NumDescs = 1;
        inputsDesc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        inputsDesc.pGeometryDescs = &geometryDesc;

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuiltInfo = {};

        device->GetRaytracingAccelerationStructurePrebuildInfo(&inputsDesc, &prebuiltInfo);

        outScratchSize =
            BLK_CEIL_TO_POWER_OF_TWO(prebuiltInfo.ScratchDataSizeInBytes,
                                     D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        outBLASSize =
            BLK_CEIL_TO_POWER_OF_TWO(prebuiltInfo.ResultDataMaxSizeInBytes,
                                     D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
    }

    void BottomLevelAS::Initialize(ComputeCommandList& commandList,
                                   D3D12_GPU_VIRTUAL_ADDRESS destination,
                                   D3D12_GPU_VIRTUAL_ADDRESS scratchBuffer,
                                   D3D12_GPU_VIRTUAL_ADDRESS vertexBuffer, UINT vertexCount,
                                   UINT vertexStride, D3D12_GPU_VIRTUAL_ADDRESS indexBuffer,
                                   UINT indexCount,
                                   D3D12_GPU_VIRTUAL_ADDRESS postBuildDataBuffer /*= NULL*/)
    {

        D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
        geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        geometryDesc.Triangles.IndexBuffer = indexBuffer;
        geometryDesc.Triangles.IndexCount = indexCount;
        geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
        geometryDesc.Triangles.Transform3x4 = 0;
        geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        geometryDesc.Triangles.VertexCount = vertexCount;
        geometryDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer;
        geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexStride;

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputsDesc = {};
        inputsDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        inputsDesc.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE |
                           D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
        inputsDesc.NumDescs = 1;
        inputsDesc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        inputsDesc.pGeometryDescs = &geometryDesc;

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
        buildDesc.DestAccelerationStructureData = destination;
        buildDesc.Inputs = inputsDesc;
        buildDesc.ScratchAccelerationStructureData = scratchBuffer;

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_DESC postBuildInfoDesc = {};
        postBuildInfoDesc.DestBuffer = postBuildDataBuffer;
        postBuildInfoDesc.InfoType =
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE;

        commandList->BuildRaytracingAccelerationStructure(&buildDesc, 1, &postBuildInfoDesc);
    }

} // namespace Boolka
