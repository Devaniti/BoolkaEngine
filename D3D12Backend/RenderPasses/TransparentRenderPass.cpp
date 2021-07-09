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
        return true; // Temporary disabled
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& depth = resourceContainer.GetTexture(ResourceContainer::Tex::GbufferDepth);
        RenderTargetView& lightBufferRTV =
            resourceContainer.GetRTV(ResourceContainer::RTV::LightBuffer);
        DepthStencilView& depthDSV = resourceContainer.GetDSV(ResourceContainer::DSV::GbufferDepth);
        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "TransparentRenderPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

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

        commandList->SetPipelineState(m_PSO.Get());

        //engineContext.GetScene().GetBatchManager().Render(commandList, renderContext,
        //                                                  BatchManager::BatchType::Transparent);

        return true;
    }

    bool TransparentRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool TransparentRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        MemoryBlock PS = DebugFileReader::ReadFile("TransparentPassPixelShader.cso");
        MemoryBlock AS = DebugFileReader::ReadFile("AmplificationShader.cso");
        MemoryBlock MS = DebugFileReader::ReadFile("MeshShader.cso");

        bool res = m_PSO.Initialize(
            device, resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default),
            ASParam{AS}, MSParam{MS}, PSParam{PS},
            RenderTargetParam{1, DXGI_FORMAT_R16G16B16A16_FLOAT}, BlendParam{true},
            DepthFormatParam{});
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_PSO.Get(), L"TransparentRenderPass::m_PSO");

        return true;
    }

    void TransparentRenderPass::Unload()
    {
        m_PSO.Unload();
    }

} // namespace Boolka
