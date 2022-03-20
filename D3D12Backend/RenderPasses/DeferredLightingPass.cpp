#include "stdafx.h"

#include "DeferredLightingPass.h"

namespace Boolka
{

    bool DeferredLightingPass::Render(RenderContext& renderContext,
                                      ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(DeferredLightingPass);
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& albedo = resourceContainer.GetTexture(ResourceContainer::Tex::GBufferAlbedo);
        Texture2D& normal = resourceContainer.GetTexture(ResourceContainer::Tex::GBufferNormal);
        Texture2D& raytraceResults =
            resourceContainer.GetTexture(ResourceContainer::Tex::GBufferRaytraceResults);
        Texture2D& depth = resourceContainer.GetTexture(ResourceContainer::Tex::GbufferDepth);
        Texture2D& lightBuffer = resourceContainer.GetTexture(ResourceContainer::Tex::LightBuffer);
        RenderTargetView& lightBufferRTV =
            resourceContainer.GetRTV(ResourceContainer::RTV::LightBuffer);
        DescriptorHeap& mainDescriptorHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        Buffer& passConstantBuffer =
            resourceContainer.GetBuffer(ResourceContainer::Buf::DeferredLighting);
        UploadBuffer& passUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::DeferredLighting);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        auto& lightContainer = frameContext.GetLightContainer();
        auto& lights = lightContainer.GetLights();

        resourceTracker.Transition(albedo, commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(normal, commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(raytraceResults, commandList,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(depth, commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(lightBuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

        for (size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
        {
            auto& shadowMap =
                resourceContainer.GetTexture(ResourceContainer::Tex::ShadowMapCube0 + lightIndex);
            resourceTracker.Transition(shadowMap, commandList,
                                       D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        commandList->OMSetRenderTargets(1, lightBufferRTV.GetCPUDescriptor(), FALSE, nullptr);
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

        commandList->SetGraphicsRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer),
            frameConstantBuffer->GetGPUVirtualAddress());
        commandList->SetGraphicsRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassConstantBuffer),
            passConstantBuffer->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetPipelineState(engineContext.GetPSOContainer()
                                          .GetPSO(PSOContainer::GraphicPSO::DeferredLighting)
                                          .Get());

        commandList->DrawInstanced(3, 1, 0, 0);

        return true;
    }

    bool DeferredLightingPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool DeferredLightingPass::Initialize(Device& device, RenderContext& renderContext)
    {
        return true;
    }

    void DeferredLightingPass::Unload()
    {
    }

} // namespace Boolka