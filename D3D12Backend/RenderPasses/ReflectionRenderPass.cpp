#include "stdafx.h"

#include "ReflectionRenderPass.h"

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/Resources/ResourceTransition.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "Containers/HLSLSharedStructures.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool ReflectionRenderPass::Render(RenderContext& renderContext,
                                      ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        RootSignature& rootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        Buffer& lightingConstantBuffer =
            resourceContainer.GetBuffer(ResourceContainer::Buf::DeferredLighting);
        Texture2D& normal = resourceContainer.GetTexture(ResourceContainer::Tex::GBufferNormal);
        Texture2D& reflections =
            resourceContainer.GetTexture(ResourceContainer::Tex::GBufferReflections);
        Texture2D& depth = resourceContainer.GetTexture(ResourceContainer::Tex::GbufferDepth);
        DescriptorHeap& mainDescriptorHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "ReflectionRenderPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        resourceTracker.Transition(reflections, commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        resourceTracker.Transition(normal, commandList,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(depth, commandList,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

        commandList->SetComputeRootSignature(rootSig.Get());
        commandList->SetPipelineState1(m_PSO.Get());
        engineContext.BindSceneResourcesCompute(commandList);
        commandList->SetComputeRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer),
            frameConstantBuffer->GetGPUVirtualAddress());
        commandList->SetComputeRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassConstantBuffer),
            lightingConstantBuffer->GetGPUVirtualAddress());

        D3D12_DISPATCH_RAYS_DESC dispatchRaysDesc{};
        dispatchRaysDesc.RayGenerationShaderRecord.StartAddress = m_ShaderTable.GetRayGenShader();
        dispatchRaysDesc.RayGenerationShaderRecord.SizeInBytes =
            D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        dispatchRaysDesc.MissShaderTable.StartAddress = m_ShaderTable.GetMissShaderTable();
        dispatchRaysDesc.MissShaderTable.SizeInBytes = 1 * D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        dispatchRaysDesc.HitGroupTable.StartAddress = m_ShaderTable.GetHitGroupTable();
        dispatchRaysDesc.HitGroupTable.SizeInBytes = 1 * D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        dispatchRaysDesc.Width = engineContext.GetBackbufferWidth();
        dispatchRaysDesc.Height = engineContext.GetBackbufferHeight();
        dispatchRaysDesc.Depth = 1;

        commandList->DispatchRays(&dispatchRaysDesc);

        return true;
    }

    bool ReflectionRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ReflectionRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();
        auto& defaultRootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);

        engineContext.ResetInitializationCommandList();
        GraphicCommandListImpl& initializationCommandList =
            engineContext.GetInitializationCommandList();

        MemoryBlock shaderLib = DebugFileReader::ReadFile("ReflectionPassLib.cso");
        const wchar_t* rayGenExport = L"ReflectionRayGeneration";
        const wchar_t* missExport = L"ReflectionMissShader";
        const wchar_t* closestHitExport = L"ReflectionClosestHit";
        const wchar_t* libExports[] = {rayGenExport, missExport, closestHitExport};

        const wchar_t* hitGroupExport = L"HitGroup";

        bool res = m_PSO.Initialize(
            device, GlobalRootSignatureParam{defaultRootSig},
            DXILLibraryParam<ARRAYSIZE(libExports)>{shaderLib, libExports},
            HitGroupParam{hitGroupExport, closestHitExport},
            RaytracingShaderConfigParam{sizeof(HLSLShared::ReflectionPayload), sizeof(Vector2)},
            RaytracingPipelineConfigParam{1});
        BLK_ASSERT_VAR(res);

        UINT64 shaderTableSize = ShaderTable::CalculateRequiredBufferSize(1, 1, 1);

        m_ShaderTableBuffer.Initialize(device, shaderTableSize, D3D12_HEAP_TYPE_DEFAULT,
                                       D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
        BLK_RENDER_DEBUG_ONLY(
            m_ShaderTableBuffer.SetDebugName(L"ReflectionRenderPass::m_ShaderTableBuffer"));

        UploadBuffer shaderTableUploadBuffer;
        shaderTableUploadBuffer.Initialize(device, shaderTableSize);
        void* uploadBuffer = shaderTableUploadBuffer.Map();
        m_ShaderTable.Build(m_ShaderTableBuffer->GetGPUVirtualAddress(), uploadBuffer, m_PSO, 1,
                            &rayGenExport, 1, &missExport, 1, &hitGroupExport);

        initializationCommandList->CopyResource(m_ShaderTableBuffer.Get(),
                                                shaderTableUploadBuffer.Get());
        ResourceTransition::Transition(initializationCommandList, m_ShaderTableBuffer,
                                       D3D12_RESOURCE_STATE_COPY_DEST,
                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

        engineContext.ExecuteInitializationCommandList(device);

        shaderTableUploadBuffer.Unload();

        return true;
    }

    void ReflectionRenderPass::Unload()
    {
        m_PSO.Unload();
        m_ShaderTableBuffer.Unload();
    }

} // namespace Boolka