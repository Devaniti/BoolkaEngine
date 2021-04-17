#include "stdafx.h"

#include "GBufferRenderPass.h"

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool GBufferRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& albedo = resourceContainer.GetTexture(ResourceContainer::Tex::GBufferAlbedo);
        Texture2D& normal = resourceContainer.GetTexture(ResourceContainer::Tex::GBufferNormal);
        Texture2D& depth = resourceContainer.GetTexture(ResourceContainer::Tex::GbufferDepth);
        RenderTargetView& albedoRTV =
            resourceContainer.GetRTV(ResourceContainer::RTV::GBufferAlbedo);
        RenderTargetView& normalRTV =
            resourceContainer.GetRTV(ResourceContainer::RTV::GBufferNormal);
        DepthStencilView& gbufferDSV =
            resourceContainer.GetDSV(ResourceContainer::DSV::GbufferDepth);
        Buffer& frameConstantBuffer =
            resourceContainer.GetFlippableBuffer(frameIndex, ResourceContainer::FlipBuf::Frame);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "GBufferRenderPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        resourceTracker.Transition(albedo, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.Transition(normal, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.Transition(depth, commandList, D3D12_RESOURCE_STATE_DEPTH_READ);

        D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[] = {*albedoRTV.GetCPUDescriptor(),
                                                       *normalRTV.GetCPUDescriptor()};
        commandList->OMSetRenderTargets(2, renderTargets, FALSE, gbufferDSV.GetCPUDescriptor());

        engineContext.GetScene().BindResources(commandList);

        UINT height = engineContext.GetBackbufferHeight();
        UINT width = engineContext.GetBackbufferWidth();

        D3D12_VIEWPORT viewportDesc = {};
        viewportDesc.Width = static_cast<float>(width);
        viewportDesc.Height = static_cast<float>(height);
        viewportDesc.MaxDepth = 1.0f;

        commandList->RSSetViewports(1, &viewportDesc);

        D3D12_RECT scissorRect = {};
        scissorRect.right = width;
        scissorRect.bottom = height;

        commandList->RSSetScissorRects(1, &scissorRect);

        const float clearColor[4] = {};
        commandList->ClearRenderTargetView(*albedoRTV.GetCPUDescriptor(), clearColor, 0, nullptr);
        commandList->ClearRenderTargetView(*normalRTV.GetCPUDescriptor(), clearColor, 0, nullptr);

        commandList->SetGraphicsRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer),
            frameConstantBuffer->GetGPUVirtualAddress());

        commandList->SetPipelineState(m_PSO.Get());

        engineContext.GetScene().GetBatchManager().Render(commandList,
                                                          BatchManager::BatchType::Opaque);

        return true;
    }

    bool GBufferRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool GBufferRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        MemoryBlock PS = DebugFileReader::ReadFile("GBufferPassPixelShader.cso");
        MemoryBlock AS = DebugFileReader::ReadFile("AmplificationShader.cso");
        MemoryBlock MS = DebugFileReader::ReadFile("MeshShader.cso");

        bool res = m_PSO.Initialize(
            device, resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default),
            ASParam(AS), MSParam(MS), PSParam(PS),
            RenderTargetParam{2, DXGI_FORMAT_R16G16B16A16_FLOAT},
            DepthStencilParam{true, false, D3D12_COMPARISON_FUNC_LESS_EQUAL}, DepthFormatParam{});
        BLK_ASSERT_VAR(res);

        return true;
    }

    void GBufferRenderPass::Unload()
    {
        m_PSO.Unload();
    }

} // namespace Boolka