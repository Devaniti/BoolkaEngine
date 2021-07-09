#include "stdafx.h"

#include "DebugMeshRenderPass.h"

namespace Boolka
{

    DebugMeshRenderPass::DebugMeshRenderPass()
        : m_CurrentAngle(0.0f)
    {
    }

    DebugMeshRenderPass::~DebugMeshRenderPass()
    {
        BLK_ASSERT(m_CurrentAngle == 0.0f);
    }

    bool DebugMeshRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        float deltaTime = renderContext.GetRenderFrameContext().GetDeltaTime();

        UINT frameIndex = renderContext.GetRenderFrameContext().GetFrameIndex();
        Texture2D& backbuffer = resourceContainer.GetBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = resourceContainer.GetBackBufferRTV(frameIndex);

        GraphicCommandListImpl& commandList =
            renderContext.GetRenderThreadContext().GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "DebugMeshRenderPass");

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE, nullptr);

        UINT height = renderContext.GetRenderEngineContext().GetBackbufferHeight();
        UINT width = renderContext.GetRenderEngineContext().GetBackbufferWidth();
        float aspectRatioCompensation = static_cast<float>(height) / width;

        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        UploadBuffer& currentUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::Frame);

        static const float rotationSpeed = BLK_FLOAT_PI / 2.0f;
        m_CurrentAngle += deltaTime * rotationSpeed;
        m_CurrentAngle = fmod(m_CurrentAngle, 2.0f * BLK_FLOAT_PI);

        float* upload = static_cast<float*>(currentUploadBuffer.Map());
        upload[0] = aspectRatioCompensation * cos(m_CurrentAngle);
        upload[1] = -sin(m_CurrentAngle);
        upload[2] = aspectRatioCompensation * sin(m_CurrentAngle);
        upload[3] = cos(m_CurrentAngle);
        currentUploadBuffer.Unmap();

        resourceTracker.Transition(frameConstantBuffer, commandList,
                                   D3D12_RESOURCE_STATE_COPY_DEST);

        commandList->CopyResource(frameConstantBuffer.Get(), currentUploadBuffer.Get());

        resourceTracker.Transition(frameConstantBuffer, commandList,
                                   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        D3D12_VIEWPORT viewportDesc = {};
        viewportDesc.Width = static_cast<float>(width);
        viewportDesc.Height = static_cast<float>(height);
        viewportDesc.MaxDepth = 1.0f;

        commandList->RSSetViewports(1, &viewportDesc);

        D3D12_RECT scissorRect = {};
        scissorRect.right = width;
        scissorRect.bottom = height;

        commandList->RSSetScissorRects(1, &scissorRect);

        const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
        commandList->ClearRenderTargetView(*backbufferRTV.GetCPUDescriptor(), clearColor, 0,
                                           nullptr);

        commandList->SetGraphicsRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer),
            frameConstantBuffer->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetPipelineState(m_PSO.Get());
        commandList->DispatchMesh(1, 1, 1);

        return true;
    }

    bool DebugMeshRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool DebugMeshRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        BLK_ASSERT(m_CurrentAngle == 0.0f);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        MemoryBlock AS{};
        MemoryBlock MS = DebugFileReader::ReadFile("DebugMeshPassMeshShader.cso");
        MemoryBlock PS = DebugFileReader::ReadFile("DebugMeshPassPixelShader.cso");

        bool res = m_PSO.Initialize(
            device, resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default),
            ASParam{AS}, MSParam{MS}, PSParam{PS});
        BLK_ASSERT_VAR(res);
        RenderDebug::SetDebugName(m_PSO.Get(), L"DebugMeshRenderPass::m_PSO");

        return true;
    }

    void DebugMeshRenderPass::Unload()
    {
        m_PSO.Unload();

        m_CurrentAngle = 0.0f;
    }

} // namespace Boolka