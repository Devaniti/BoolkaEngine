#include "stdafx.h"

#include "GBufferRenderPass.h"

namespace Boolka
{

    bool GBufferRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(GBufferRenderPass);

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
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        resourceTracker.Transition(albedo, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.Transition(normal, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.Transition(depth, commandList, D3D12_RESOURCE_STATE_DEPTH_READ);

        D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[] = {*albedoRTV.GetCPUDescriptor(),
                                                       *normalRTV.GetCPUDescriptor()};
        commandList->OMSetRenderTargets(2, renderTargets, FALSE, gbufferDSV.GetCPUDescriptor());

        engineContext.BindSceneResourcesGraphic(commandList);

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

        commandList->SetPipelineState(
            engineContext.GetPSOContainer().GetPSO(PSOContainer::GraphicPSO::GBuffer).Get());

        engineContext.GetScene().GetBatchManager().Render(commandList, renderContext,
                                                          BatchManager::BatchType::Opaque);

        return true;
    }

    bool GBufferRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool GBufferRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        return true;
    }

    void GBufferRenderPass::Unload()
    {
    }

} // namespace Boolka