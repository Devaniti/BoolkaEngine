#include "stdafx.h"

#include "PSOContainer.h"

#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"

namespace Boolka
{

    bool PSOContainer::Initialize(Device& device, RenderEngineContext& engineContext)
    {
        BLK_CPU_SCOPE("PSOContainer::Initialize");

        DebugProfileTimer timer;
        timer.Start();

        auto& resourceContainer = engineContext.GetResourceContainer();
        auto& defaultRootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);

        InputLayout emptyInputLayout;
        emptyInputLayout.Initialize(0);
        BLK_RENDER_DEBUG_ONLY(
            device.FilterMessage(D3D12_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT));

        MemoryBlock PS;
        MemoryBlock AS;
        MemoryBlock MS;
        MemoryBlock VS;
        MemoryBlock CS;
        bool res;

        PS = DebugFileReader::ReadFile("DeferredLightingPassPS.cso");
        VS = DebugFileReader::ReadFile("FullScreenVS.cso");
        res = GetPSO(GraphicPSO::DeferredLighting)
                  .Initialize(device, L"GraphicPSO::DeferredLighting", defaultRootSig,
                              emptyInputLayout, VSParam{VS}, PSParam{PS},
                              RenderTargetParam{1, DXGI_FORMAT_R16G16B16A16_FLOAT},
                              DepthStencilParam{false, false, D3D12_COMPARISON_FUNC_ALWAYS},
                              DepthFormatParam{});
        BLK_ASSERT_VAR(res);
        DebugFileReader::FreeMemory(PS);
        DebugFileReader::FreeMemory(VS);
        emptyInputLayout.Unload();

        PS = DebugFileReader::ReadFile("GBufferPassPixelShader.cso");
        AS = DebugFileReader::ReadFile("AmplificationShader.cso");
        MS = DebugFileReader::ReadFile("MeshShader.cso");
        res = GetPSO(GraphicPSO::GBuffer)
                  .Initialize(device, L"GraphicPSO::GBuffer", defaultRootSig, ASParam(AS),
                              MSParam(MS), PSParam(PS),
                              RenderTargetParam{2, DXGI_FORMAT_R16G16B16A16_FLOAT},
                              DepthStencilParam{true, false, D3D12_COMPARISON_FUNC_LESS_EQUAL},
                              DepthFormatParam{});
        BLK_ASSERT_VAR(res);
        DebugFileReader::FreeMemory(PS);
        DebugFileReader::FreeMemory(AS);
        DebugFileReader::FreeMemory(MS);

        AS = DebugFileReader::ReadFile("ShadowMapAmplificationShader.cso");
        MS = DebugFileReader::ReadFile("ShadowMapPassMeshShader.cso");
        res = GetPSO(GraphicPSO::ShadowMap)
                  .Initialize(device, L"GraphicPSO::ShadowMap", defaultRootSig, ASParam{AS},
                              MSParam{MS}, RenderTargetParam{0},
                              DepthStencilParam{true, true, D3D12_COMPARISON_FUNC_LESS},
                              DepthFormatParam{}, RasterizerParam{0.001f, 0.0f});
        BLK_ASSERT_VAR(res);
        DebugFileReader::FreeMemory(AS);
        DebugFileReader::FreeMemory(MS);

        PS = DebugFileReader::ReadFile("SkyBoxPassPixelShader.cso");
        VS = DebugFileReader::ReadFile("FullScreenVS.cso");
        res = GetPSO(GraphicPSO::SkyBox)
                  .Initialize(device, L"GraphicPSO::SkyBox", defaultRootSig, emptyInputLayout,
                              VSParam{VS}, PSParam{PS},
                              RenderTargetParam{1, DXGI_FORMAT_R16G16B16A16_FLOAT},
                              DepthStencilParam{true, false, D3D12_COMPARISON_FUNC_EQUAL},
                              DepthFormatParam{});
        BLK_ASSERT_VAR(res);
        DebugFileReader::FreeMemory(PS);
        DebugFileReader::FreeMemory(VS);

        PS = DebugFileReader::ReadFile("ToneMappingPassPS.cso");
        VS = DebugFileReader::ReadFile("FullScreenVS.cso");
        res = GetPSO(GraphicPSO::ToneMapping)
                  .Initialize(device, L"GraphicPSO::ToneMapping", defaultRootSig, emptyInputLayout,
                              VSParam{VS}, PSParam{PS},
                              DepthStencilParam{false, false, D3D12_COMPARISON_FUNC_ALWAYS},
                              DepthFormatParam{});
        BLK_ASSERT_VAR(res);
        DebugFileReader::FreeMemory(PS);
        DebugFileReader::FreeMemory(VS);

        AS = DebugFileReader::ReadFile("AmplificationShader.cso");
        MS = DebugFileReader::ReadFile("MeshShader.cso");
        res = GetPSO(GraphicPSO::ZBuffer)
                  .Initialize(device, L"GraphicPSO::ZBuffer", defaultRootSig, ASParam{AS},
                              MSParam{MS}, RenderTargetParam{0}, DepthStencilParam{true, true},
                              DepthFormatParam{});
        BLK_ASSERT_VAR(res);
        DebugFileReader::FreeMemory(AS);
        DebugFileReader::FreeMemory(MS);

        CS = DebugFileReader::ReadFile("GPUCullingComputeShader.cso");
        res = GetPSO(ComputePSO::GPUCulling)
                  .Initialize(device, L"ComputePSO::GPUCullingComputeShader", defaultRootSig, CS);
        BLK_ASSERT_VAR(res);
        DebugFileReader::FreeMemory(CS);

        if (device.SupportsRaytracing())
        {
            DebugProfileTimer rtpsoTimer;
            rtpsoTimer.Start();

            MemoryBlock shaderLib = DebugFileReader::ReadFile("RaytracePassLib.cso");
            const wchar_t* rayGenExport = L"RayGeneration";
            const wchar_t* missExport = L"MissShader";
            const wchar_t* closestHitExport = L"ClosestHit";
            const wchar_t* libExports[] = {rayGenExport, missExport, closestHitExport};
            const wchar_t* hitGroupExport = L"HitGroup";
            res = GetPSO(RTPSO::RaytracePass)
                      .Initialize(device, L"RTPSO::RaytracePass",
                                  GlobalRootSignatureParam{defaultRootSig},
                                  DXILLibraryParam<ARRAYSIZE(libExports)>{shaderLib, libExports},
                                  HitGroupParam{hitGroupExport, closestHitExport},
                                  RaytracingShaderConfigParam{sizeof(HLSLShared::RaytracePayload),
                                                              sizeof(Vector2)},
                                  RaytracingPipelineConfigParam{BLK_RT_MAX_RECURSION_DEPTH});
            BLK_ASSERT_VAR(res);
            DebugFileReader::FreeMemory(shaderLib);

            rtpsoTimer.Stop(L"RTPSO compile");

            UINT64 shaderTableSize = ShaderTable::CalculateRequiredBufferSize(1, 1, 1);

            Buffer& shaderTableBuffer = engineContext.GetResourceContainer().GetBuffer(
                ResourceContainer::Buf::RTShaderTable);
            m_ShaderTablesUploadBuffer.Initialize(device, shaderTableSize);
            void* uploadBuffer = m_ShaderTablesUploadBuffer.Map();
            GetShaderTable(RTShaderTable::Default)
                .Build(shaderTableBuffer->GetGPUVirtualAddress(), uploadBuffer,
                       GetPSO(RTPSO::RaytracePass), 1, &rayGenExport, 1, &missExport, 1,
                       &hitGroupExport);

            GraphicCommandListImpl& initializationCommandList =
                engineContext.GetInitializationCommandList();
            initializationCommandList->CopyResource(shaderTableBuffer.Get(),
                                                    m_ShaderTablesUploadBuffer.Get());

            BLK_RENDER_DEBUG_ONLY(device.RemoveLastMessageFilter());

            timer.Stop(L"All PSOs compile");
        }

        return true;
    }

    void PSOContainer::Unload()
    {
        BLK_UNLOAD_ARRAY(m_GraphicPSOs);
        BLK_UNLOAD_ARRAY(m_ComputePSOs);
        BLK_SAFE_UNLOAD_ARRAY(m_RTPSOs);
    }

    void PSOContainer::FinishInitialization(Device& device)
    {
        if (device.SupportsRaytracing())
        {
			m_ShaderTablesUploadBuffer.Unload();
		}
    }

    GraphicPipelineState& PSOContainer::GetPSO(GraphicPSO id)
    {
        return m_GraphicPSOs[static_cast<size_t>(id)];
    }

    ComputePipelineState& PSOContainer::GetPSO(ComputePSO id)
    {
        return m_ComputePSOs[static_cast<size_t>(id)];
    }

    StateObject& PSOContainer::GetPSO(RTPSO id)
    {
        return m_RTPSOs[static_cast<size_t>(id)];
    }

    ShaderTable& PSOContainer::GetShaderTable(RTShaderTable id)
    {
        return m_RTShaderTables[static_cast<size_t>(id)];
    }

} // namespace Boolka
