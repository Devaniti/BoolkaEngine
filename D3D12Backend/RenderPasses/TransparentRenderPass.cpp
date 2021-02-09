#include "stdafx.h"
#include "TransparentRenderPass.h"

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

    bool TransparentRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& backbuffer = engineContext.GetSwapchainBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = engineContext.GetSwapchainRenderTargetView(frameIndex);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "TransparentRenderPass");

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE, engineContext.GetDepthStencilView().GetCPUDescriptor());
        ID3D12DescriptorHeap* descriptorHeaps[] = { engineContext.GetScene().GetSRVDescriptorHeap().Get() };
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(6, engineContext.GetScene().GetSRVDescriptorHeap().GetGPUHandle(0));

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

        commandList->SetGraphicsRootConstantBufferView(0, currentConstantBuffer->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetIndexBuffer(engineContext.GetScene().GetIndexBufferView().GetView());

        commandList->IASetVertexBuffers(0, 1, engineContext.GetScene().GetVertexBufferView().GetView());
        commandList->SetPipelineState(m_PSO.Get());

        engineContext.GetScene().GetBatchManager().Render(commandList, BatchManager::BatchType::Transparent);

        return true;
    }

    bool TransparentRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool TransparentRenderPass::Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        MemoryBlock PS = DebugFileReader::ReadFile("TransparentPassPixelShader.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("TransparentPassVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(4);
        inputLayout.SetEntry(0, { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(1, { "MATERIAL", 0, DXGI_FORMAT_R32_SINT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(2, { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(3, { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

        Scene& scene = engineContext.GetScene();

        bool res = m_PSO.Initialize(device, engineContext.GetDefaultRootSig(), inputLayout, VS, PS, 1, true, true, D3D12_COMPARISON_FUNC_LESS, true);
        BLK_ASSERT(res);

        inputLayout.Unload();

        return true;
    }

    void TransparentRenderPass::Unload()
    {
        m_PSO.Unload();
    }

}