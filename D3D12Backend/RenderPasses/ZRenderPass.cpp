#include "stdafx.h"

#include "ZRenderPass.h"

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/RenderDebug.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool ZRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(ZRenderPass);
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& gbufferDepth =
            resourceContainer.GetTexture(ResourceContainer::Tex2D::GbufferDepth);
        DepthStencilView& gbufferDSV =
            resourceContainer.GetDSV(ResourceContainer::DSV::GbufferDepth);
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        resourceTracker.Transition(gbufferDepth, commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);

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

        commandList->ClearDepthStencilView(*gbufferDSV.GetCPUDescriptor(), D3D12_CLEAR_FLAG_DEPTH,
                                           1.0f, 0, 0, nullptr);
        commandList->OMSetRenderTargets(0, nullptr, FALSE, gbufferDSV.GetCPUDescriptor());

        commandList->SetGraphicsRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer),
            frameConstantBuffer->GetGPUVirtualAddress());

        engineContext.BindSceneResourcesGraphic(commandList);

        commandList->SetPipelineState(
            engineContext.GetPSOContainer().GetPSO(PSOContainer::GraphicPSO::ZBuffer).Get());

        engineContext.GetScene().GetBatchManager().Render(commandList, renderContext,
                                                          BatchManager::BatchType::Opaque);

        return true;
    }

    bool ZRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ZRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        return true;
    }

    void ZRenderPass::Unload()
    {
    }

} // namespace Boolka