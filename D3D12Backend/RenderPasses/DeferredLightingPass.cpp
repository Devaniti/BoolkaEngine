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
        Texture2D& reflections =
            resourceContainer.GetTexture(ResourceContainer::Tex::GBufferReflections);
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
        resourceTracker.Transition(reflections, commandList,
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
        commandList->SetPipelineState(m_PSO.Get());

        commandList->DrawInstanced(3, 1, 0, 0);

        return true;
    }

    bool DeferredLightingPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool DeferredLightingPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        MemoryBlock PS = DebugFileReader::ReadFile("DeferredLightingPassPS.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("FullScreenVS.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(0);

        // We are intentionally using empty input layout here to draw fullscreen
        // quad without Vertex/Index buffers
        BLK_RENDER_DEBUG_ONLY(
            device.FilterMessage(D3D12_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT));
        bool res = m_PSO.Initialize(
            device, resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default),
            inputLayout, VSParam{VS}, PSParam{PS},
            RenderTargetParam{1, DXGI_FORMAT_R16G16B16A16_FLOAT},
            DepthStencilParam{false, false, D3D12_COMPARISON_FUNC_ALWAYS}, DepthFormatParam{});
        BLK_RENDER_DEBUG_ONLY(device.RemoveLastMessageFilter());
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_PSO.Get(), L"DeferredLightingPass::m_PSO");

        inputLayout.Unload();

        return true;
    }

    void DeferredLightingPass::Unload()
    {
        m_PSO.Unload();
    }

} // namespace Boolka