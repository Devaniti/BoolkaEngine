#include "stdafx.h"

#include "TransparentRenderPass.h"

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

    bool TransparentRenderPass::Render(RenderContext& renderContext,
                                       ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(TransparentRenderPass);
        return true; // Temporary disabled
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& depth = resourceContainer.GetTexture(ResourceContainer::Tex2D::GbufferDepth);
        RenderTargetView& lightBufferRTV =
            resourceContainer.GetRTV(ResourceContainer::RTV::LightBuffer);
        DepthStencilView& depthDSV = resourceContainer.GetDSV(ResourceContainer::DSV::GbufferDepth);
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        resourceTracker.Transition(depth, commandList, D3D12_RESOURCE_STATE_DEPTH_READ);

        commandList->OMSetRenderTargets(1, lightBufferRTV.GetCPUDescriptor(), FALSE,
                                        depthDSV.GetCPUDescriptor());
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

        engineContext.BindSceneResourcesGraphic(commandList);

        // commandList->SetPipelineState(TODO);

        // engineContext.GetScene().GetBatchManager().Render(commandList, renderContext,
        //                                                  BatchManager::BatchType::Transparent);

        return true;
    }

    bool TransparentRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool TransparentRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        return true;
    }

    void TransparentRenderPass::Unload()
    {
    }

} // namespace Boolka
