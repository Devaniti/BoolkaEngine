#include "stdafx.h"
#include "ZRenderPass.h"

#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "APIWrappers/InputLayout.h"
#include "RenderSchedule/ResourceTracker.h"
#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderThreadContext.h"

namespace Boolka
{

    bool ZRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        UINT frameIndex = frameContext.GetFrameIndex();
        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "ZRenderPass");

        UINT height = engineContext.GetBackbufferHeight();
        UINT width = engineContext.GetBackbufferWidth();

        Buffer& currentConstantBuffer = engineContext.GetConstantBuffer(frameIndex);

        D3D12_VIEWPORT viewportDesc = {};
        viewportDesc.Width = static_cast<float>(width);
        viewportDesc.Height = static_cast<float>(height);
        viewportDesc.MaxDepth = 1.0f;

        commandList->RSSetViewports(1, &viewportDesc);

        D3D12_RECT scissorRect = {};
        scissorRect.right = width;
        scissorRect.bottom = height;

        commandList->RSSetScissorRects(1, &scissorRect);

        commandList->ClearDepthStencilView(*engineContext.GetDepthStencilView().GetCPUDescriptor(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        commandList->OMSetRenderTargets(0, nullptr, FALSE, engineContext.GetDepthStencilView().GetCPUDescriptor());

        commandList->SetGraphicsRootConstantBufferView(0, currentConstantBuffer->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetIndexBuffer(engineContext.GetScene().GetIndexBufferView().GetView());

        commandList->IASetVertexBuffers(0, 1, engineContext.GetScene().GetVertexBufferView().GetView());
        commandList->SetPipelineState(m_PSO.Get());

        engineContext.GetScene().GetBatchManager().Render(commandList, BatchManager::BatchType::Opaque);

        return true;
    }

    bool ZRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ZRenderPass::Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        MemoryBlock PS = {};
        MemoryBlock VS = DebugFileReader::ReadFile("ZPassVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(1);
        inputLayout.SetEntry(0, { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

        Scene& scene = engineContext.GetScene();

        bool res = m_PSO.Initialize(device, engineContext.GetDefaultRootSig(), inputLayout, VS, PS, 0, true);
        BLK_ASSERT(res);

        inputLayout.Unload();

        return true;
    }

    void ZRenderPass::Unload()
    {
        m_PSO.Unload();
    }

}