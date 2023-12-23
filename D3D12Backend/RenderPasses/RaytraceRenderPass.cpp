#include "stdafx.h"

#include "RaytraceRenderPass.h"

#include "APIWrappers/RenderDebug.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/Resources/ResourceTransition.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool RaytraceRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(RaytraceRenderPass);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        if (!m_Enabled)
        {
            return true;
        }

        RootSignature& rootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        Buffer& lightingConstantBuffer =
            resourceContainer.GetBuffer(ResourceContainer::Buf::DeferredLighting);
        Texture2D& normal = resourceContainer.GetTexture(ResourceContainer::Tex::GBufferNormal);
        Texture2D& raytraceResults =
            resourceContainer.GetTexture(ResourceContainer::Tex::GBufferRaytraceResults);
        Texture2D& depth = resourceContainer.GetTexture(ResourceContainer::Tex::GbufferDepth);
        DescriptorHeap& mainDescriptorHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        resourceTracker.Transition(raytraceResults, commandList,
                                   D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        resourceTracker.Transition(normal, commandList,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(depth, commandList,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

        commandList->SetComputeRootSignature(rootSig.Get());
        commandList->SetPipelineState1(
            engineContext.GetPSOContainer().GetPSO(PSOContainer::RTPSO::RaytracePass).Get());
        engineContext.BindSceneResourcesCompute(commandList);
        commandList->SetComputeRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer),
            frameConstantBuffer->GetGPUVirtualAddress());
        commandList->SetComputeRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassConstantBuffer),
            lightingConstantBuffer->GetGPUVirtualAddress());

        ShaderTable& shaderTable =
            engineContext.GetPSOContainer().GetShaderTable(PSOContainer::RTShaderTable::Default);

        D3D12_DISPATCH_RAYS_DESC dispatchRaysDesc{};
        dispatchRaysDesc.RayGenerationShaderRecord.StartAddress = shaderTable.GetRayGenShader();
        dispatchRaysDesc.RayGenerationShaderRecord.SizeInBytes =
            D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        dispatchRaysDesc.MissShaderTable.StartAddress = shaderTable.GetMissShaderTable();
        dispatchRaysDesc.MissShaderTable.SizeInBytes = 1 * D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        dispatchRaysDesc.HitGroupTable.StartAddress = shaderTable.GetHitGroupTable();
        dispatchRaysDesc.HitGroupTable.SizeInBytes = 1 * D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        dispatchRaysDesc.Width = engineContext.GetBackbufferWidth();
        dispatchRaysDesc.Height = engineContext.GetBackbufferHeight();
        dispatchRaysDesc.Depth = 1;

        commandList->DispatchRays(&dispatchRaysDesc);

        return true;
    }

    bool RaytraceRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool RaytraceRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        m_Enabled = device.SupportsRaytracing();
        return true;
    }

    void RaytraceRenderPass::Unload()
    {
    }

} // namespace Boolka