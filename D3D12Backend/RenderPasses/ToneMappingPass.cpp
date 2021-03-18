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
#include "RenderSchedule/ResourceContainer.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool ToneMappingPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& lightBuffer = resourceContainer.GetTexture(ResourceContainer::Tex::LightBuffer);
        Texture2D& backBuffer = resourceContainer.GetBackBuffer(frameIndex);
        RenderTargetView& backBufferRTV = resourceContainer.GetBackBufferRTV(frameIndex);
        DescriptorHeap& mainDescriptorHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        Buffer& frameConstantBuffer =
            resourceContainer.GetFlippableBuffer(frameIndex, ResourceContainer::FlipBuf::Frame);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "ToneMappingPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        resourceTracker.Transition(lightBuffer, commandList,
                                   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(backBuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

        commandList->OMSetRenderTargets(1, backBufferRTV.GetCPUDescriptor(), FALSE, nullptr);
        ID3D12DescriptorHeap* descriptorHeaps[] = {mainDescriptorHeap.Get()};
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::RenderPassSRV),
            mainDescriptorHeap.GetGPUHandle(0));

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

    bool ToneMappingPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ToneMappingPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();
        auto& defaultRootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);

        MemoryBlock PS = DebugFileReader::ReadFile("ToneMappingPassPS.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("FullScreenVS.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(0);

        // We are intentionally using empty input layout here to draw fullscreen
        // quad without Vertex/Index buffers
        BLK_RENDER_DEBUG_ONLY(
            device.FilterMessage(D3D12_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT));
        bool res = m_PSO.Initialize(device, defaultRootSig, inputLayout, VS, PS, 1, false, false,
                                    D3D12_COMPARISON_FUNC_ALWAYS, false);
        BLK_RENDER_DEBUG_ONLY(device.RemoveLastMessageFilter());
        BLK_ASSERT_VAR(res);

        inputLayout.Unload();

        return true;
    }

    void ToneMappingPass::Unload()
    {
        m_PSO.Unload();
    }

} // namespace Boolka