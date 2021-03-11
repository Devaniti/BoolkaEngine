#include "stdafx.h"

#include "ZRenderPass.h"

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

    bool ZRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& gbufferDepth =
            resourceContainer.GetTexture(ResourceContainer::Tex::GbufferDepth);
        DepthStencilView& gbufferDSV =
            resourceContainer.GetDSV(ResourceContainer::DSV::GbufferDepth);
        Buffer& frameConstantBuffer =
            resourceContainer.GetFlippableBuffer(frameIndex, ResourceContainer::FlipBuf::Frame);
        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "ZRenderPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

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

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetIndexBuffer(engineContext.GetScene().GetIndexBufferView().GetView());

        commandList->IASetVertexBuffers(0, 1,
                                        engineContext.GetScene().GetVertexBufferView().GetView());
        commandList->SetPipelineState(m_PSO.Get());

        engineContext.GetScene().GetBatchManager().Render(commandList,
                                                          BatchManager::BatchType::Opaque);

        return true;
    }

    bool ZRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ZRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        MemoryBlock PS = {};
        MemoryBlock VS = DebugFileReader::ReadFile("ZPassVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(1);
        inputLayout.SetEntry(0, {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        bool res = m_PSO.Initialize(
            device, resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default),
            inputLayout, VS, PS, 0, true);
        BLK_ASSERT(res);

        inputLayout.Unload();

        return true;
    }

    void ZRenderPass::Unload()
    {
        m_PSO.Unload();
    }

} // namespace Boolka