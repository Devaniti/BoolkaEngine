#include "stdafx.h"

#include "ShaderTable.h"

#include "APIWrappers/PipelineState/StateObject.h"

namespace Boolka
{

    UINT64 ShaderTable::CalculateRequiredBufferSize(UINT rayGenShaderCount, UINT missShaderCount,
                                                    UINT closestHitShaderCount)
    {
        const UINT64 shaderIDSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        const UINT64 shaderTableAlignment = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
        return BLK_CEIL_TO_POWER_OF_TWO(rayGenShaderCount * shaderIDSize, shaderTableAlignment) +
               BLK_CEIL_TO_POWER_OF_TWO(missShaderCount * shaderIDSize, shaderTableAlignment) +
               BLK_CEIL_TO_POWER_OF_TWO(closestHitShaderCount * shaderIDSize, shaderTableAlignment);
    }

    void ShaderTable::Build(D3D12_GPU_VIRTUAL_ADDRESS destGPUBufferAdress, void* uploadDest,
                            StateObject& stateObject, UINT rayGenShaderCount,
                            const wchar_t** rayGenShaderExports, UINT missShaderCount,
                            const wchar_t** missShaderExports, UINT closestHitShaderCount,
                            const wchar_t** closestHitShaderExports)
    {
        const UINT64 shaderIDSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        const UINT64 shaderTableAlignment = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;

        ID3D12StateObjectProperties* stateObjectProperties = nullptr;
        HRESULT hr = stateObject->QueryInterface(IID_PPV_ARGS(&stateObjectProperties));
        BLK_ASSERT_VAR(SUCCEEDED(hr));

        char* uploadDestBytes = static_cast<char*>(uploadDest);

        m_RayGenShaderTable = destGPUBufferAdress;
        destGPUBufferAdress +=
            BLK_CEIL_TO_POWER_OF_TWO(shaderIDSize * rayGenShaderCount, shaderTableAlignment);
        m_MissShaderTable = destGPUBufferAdress;
        destGPUBufferAdress +=
            BLK_CEIL_TO_POWER_OF_TWO(shaderIDSize * missShaderCount, shaderTableAlignment);
        m_HitGroupTable = destGPUBufferAdress;

        for (UINT i = 0; i < rayGenShaderCount; ++i)
        {
            void* shaderID = stateObjectProperties->GetShaderIdentifier(rayGenShaderExports[i]);
            memcpy(uploadDestBytes + shaderIDSize * i, shaderID, shaderIDSize);
        }
        uploadDestBytes +=
            BLK_CEIL_TO_POWER_OF_TWO(shaderIDSize * rayGenShaderCount, shaderTableAlignment);

        for (UINT i = 0; i < missShaderCount; ++i)
        {
            void* shaderID = stateObjectProperties->GetShaderIdentifier(missShaderExports[i]);
            memcpy(uploadDestBytes + shaderIDSize * i, shaderID, shaderIDSize);
        }
        uploadDestBytes +=
            BLK_CEIL_TO_POWER_OF_TWO(shaderIDSize * missShaderCount, shaderTableAlignment);

        for (UINT i = 0; i < closestHitShaderCount; ++i)
        {
            void* shaderID = stateObjectProperties->GetShaderIdentifier(closestHitShaderExports[i]);
            memcpy(uploadDestBytes + shaderIDSize * i, shaderID, shaderIDSize);
        }

        stateObjectProperties->Release();
    }

    D3D12_GPU_VIRTUAL_ADDRESS ShaderTable::GetRayGenShader() const
    {
        return m_RayGenShaderTable;
    }

    D3D12_GPU_VIRTUAL_ADDRESS ShaderTable::GetMissShaderTable() const
    {
        return m_MissShaderTable;
    }

    D3D12_GPU_VIRTUAL_ADDRESS ShaderTable::GetHitGroupTable() const
    {
        return m_HitGroupTable;
    }

} // namespace Boolka
