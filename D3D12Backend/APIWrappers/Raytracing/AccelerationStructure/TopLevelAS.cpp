#include "stdafx.h"

#include "TopLevelAS.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    void TopLevelAS::GetSizes(Device& device, UINT blasInstanceCount, UINT64& outScratchSize,
                              UINT64& outTLASSize)
    {
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputsDesc = {};
        inputsDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        inputsDesc.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        inputsDesc.NumDescs = blasInstanceCount;
        inputsDesc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuiltInfo = {};

        device->GetRaytracingAccelerationStructurePrebuildInfo(&inputsDesc, &prebuiltInfo);

        outScratchSize =
            BLK_CEIL_TO_POWER_OF_TWO(prebuiltInfo.ScratchDataSizeInBytes,
                                     D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        outTLASSize =
            BLK_CEIL_TO_POWER_OF_TWO(prebuiltInfo.ResultDataMaxSizeInBytes,
                                     D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
    }

    void TopLevelAS::Initialize(ComputeCommandList& commandList,
                                D3D12_GPU_VIRTUAL_ADDRESS destination,
                                D3D12_GPU_VIRTUAL_ADDRESS scratchBuffer,
                                D3D12_GPU_VIRTUAL_ADDRESS blasInstances, UINT instanceCount)
    {
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputsDesc = {};
        inputsDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        inputsDesc.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        inputsDesc.NumDescs = instanceCount;
        inputsDesc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
        inputsDesc.InstanceDescs = blasInstances;

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
        buildDesc.DestAccelerationStructureData = destination;
        buildDesc.Inputs = inputsDesc;
        buildDesc.ScratchAccelerationStructureData = scratchBuffer;

        commandList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
    }

} // namespace Boolka
