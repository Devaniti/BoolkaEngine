#include "stdafx.h"

#include "ToneMappingPass.h"

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

    bool ToneMappingPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(ToneMappingPass);
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& lightBuffer = resourceContainer.GetTexture(ResourceContainer::Tex::LightBuffer);
        Texture2D& backBuffer = resourceContainer.GetBackBuffer(frameIndex);
        RenderTargetView& backBufferRTV = resourceContainer.GetBackBufferRTV(frameIndex);
        DescriptorHeap& mainDescriptorHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        resourceTracker.Transition(lightBuffer, commandList,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(backBuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

        commandList->OMSetRenderTargets(1, backBufferRTV.GetCPUDescriptor(), FALSE, nullptr);
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
        commandList->SetPipelineState(
            engineContext.GetPSOContainer().GetPSO(PSOContainer::GraphicPSO::ToneMapping).Get());

        commandList->DrawInstanced(3, 1, 0, 2);

        return true;
    }

    bool ToneMappingPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ToneMappingPass::Initialize(Device& device, RenderContext& renderContext)
    {
        return true;
    }

    void ToneMappingPass::Unload()
    {
    }

} // namespace Boolka