#include "stdafx.h"

#include "SkyBoxRenderPass.h"

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/Device.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
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

    bool SkyBoxRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& depth = resourceContainer.GetTexture(ResourceContainer::Tex::GbufferDepth);
        RenderTargetView& lightBufferRTV =
            resourceContainer.GetRTV(ResourceContainer::RTV::LightBuffer);
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        DepthStencilView& gbufferDSV =
            resourceContainer.GetDSV(ResourceContainer::DSV::GbufferDepth);
        auto& scene = engineContext.GetScene();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "SkyBoxRenderPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        resourceTracker.Transition(depth, commandList, D3D12_RESOURCE_STATE_DEPTH_READ);

        commandList->OMSetRenderTargets(1, lightBufferRTV.GetCPUDescriptor(), FALSE,
                                        gbufferDSV.GetCPUDescriptor());
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

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetPipelineState(m_PSO.Get());

        commandList->DrawInstanced(3, 1, 0, 0);

        return true;
    }

    bool SkyBoxRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool SkyBoxRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        MemoryBlock PS = DebugFileReader::ReadFile("SkyBoxPassPixelShader.cso");
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
            DepthStencilParam{true, false, D3D12_COMPARISON_FUNC_EQUAL}, DepthFormatParam{});
        BLK_RENDER_DEBUG_ONLY(device.RemoveLastMessageFilter());
        BLK_ASSERT_VAR(res);

        inputLayout.Unload();

        return true;
    }

    void SkyBoxRenderPass::Unload()
    {
        m_PSO.Unload();
    }

} // namespace Boolka