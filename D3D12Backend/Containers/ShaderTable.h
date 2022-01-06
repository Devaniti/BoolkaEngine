#pragma once

namespace Boolka
{

    class StateObject;

    class [[nodiscard]] ShaderTable
    {
    public:
        [[nodiscard]] static UINT64 CalculateRequiredBufferSize(UINT rayGenShaderCount,
                                                                UINT missShaderCount,
                                                                UINT closestHitShaderCount);
        void Build(D3D12_GPU_VIRTUAL_ADDRESS destGPUBufferAdress, void* uploadDest,
                   StateObject& stateObject, UINT rayGenShaderCount,
                   const wchar_t** rayGenShaderExports, UINT missShaderCount,
                   const wchar_t** missShaderExports, UINT hitGroupCount,
                   const wchar_t** hitGroupExports);

        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetRayGenShader() const;
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetMissShaderTable() const;
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetHitGroupTable() const;

    private:
        D3D12_GPU_VIRTUAL_ADDRESS m_RayGenShaderTable;
        D3D12_GPU_VIRTUAL_ADDRESS m_MissShaderTable;
        D3D12_GPU_VIRTUAL_ADDRESS m_HitGroupTable;
    };

} // namespace Boolka
