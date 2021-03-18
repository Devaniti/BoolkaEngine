#include "stdafx.h"

#include "DebugRenderPass.h"

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/Resources/ResourceTransition.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    DebugRenderPass::DebugRenderPass()
        : m_CurrentAngle(0.0f)
    {
    }

    DebugRenderPass::~DebugRenderPass()
    {
        BLK_ASSERT(m_CurrentAngle == 0.0f);
    }

    bool DebugRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        float deltaTime = renderContext.GetRenderFrameContext().GetDeltaTime();

        UINT frameIndex = renderContext.GetRenderFrameContext().GetFrameIndex();
        Texture2D& backbuffer = resourceContainer.GetBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = resourceContainer.GetBackBufferRTV(frameIndex);

        GraphicCommandListImpl& commandList =
            renderContext.GetRenderThreadContext().GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "DebugRenderPass");

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE, nullptr);

        UINT height = renderContext.GetRenderEngineContext().GetBackbufferHeight();
        UINT width = renderContext.GetRenderEngineContext().GetBackbufferWidth();
        float aspectRatioCompensation = static_cast<float>(height) / width;

        Buffer& frameConstantBuffer =
            resourceContainer.GetFlippableBuffer(frameIndex, ResourceContainer::FlipBuf::Frame);
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

        commandList->CopyResource(frameConstantBuffer.Get(), currentUploadBuffer.Get());

        ResourceTransition::Transition(frameConstantBuffer, commandList,
                                       D3D12_RESOURCE_STATE_COPY_DEST,
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
        commandList->IASetIndexBuffer(m_IndexBufferView.GetView());
        commandList->IASetVertexBuffers(0, 1, m_VertexBufferView.GetView());
        commandList->SetPipelineState(m_PSO.Get());
        commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);

        ResourceTransition::Transition(frameConstantBuffer, commandList,
                                       D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                                       D3D12_RESOURCE_STATE_COPY_DEST);

        return true;
    }

    bool DebugRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool DebugRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        BLK_ASSERT(m_CurrentAngle == 0.0f);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        MemoryBlock PS = DebugFileReader::ReadFile("DebugPassPixelShader.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("DebugPassVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(2);
        inputLayout.SetEntry(0, {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
        inputLayout.SetEntry(1, {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8,
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        static const UINT64 vertexCount = 3;

        static const float a = 0.8f;

        struct Vertex
        {
            float position[2];
            float color[3];
        } vertexData[vertexCount] = {{{-sqrt(3.0f) / 2.0f * a, -a / 2.0f}, {1.0f, 0.0f, 0.0f}},
                                     {{sqrt(3.0f) / 2.0f * a, -a / 2.0f}, {0.0f, 1.0f, 0.0f}},
                                     {{0.0f, a}, {0.0f, 0.0f, 1.0f}}};

        static const UINT64 vertexSize = sizeof(Vertex);
        static const UINT64 vertexBufferSize = vertexSize * vertexCount;

        bool res = m_VertexBuffer.Initialize(device, vertexBufferSize);
        BLK_ASSERT_VAR(res);
        m_VertexBuffer.Upload(vertexData, vertexBufferSize);

        res = m_VertexBufferView.Initialize(m_VertexBuffer, vertexBufferSize, vertexSize);
        BLK_ASSERT_VAR(res);

        static const UINT64 indexCount = 3;

        uint16_t indexData[indexCount] = {0, 1, 2};

        static const UINT64 indexSize = sizeof(uint16_t);
        static const UINT64 indexBufferSize = indexSize * indexCount;

        res = m_IndexBuffer.Initialize(device, indexBufferSize);
        BLK_ASSERT_VAR(res);
        m_IndexBuffer.Upload(indexData, indexBufferSize);

        res = m_IndexBufferView.Initialize(m_IndexBuffer, indexBufferSize, DXGI_FORMAT_R16_UINT);
        BLK_ASSERT_VAR(res);

        res = m_PSO.Initialize(
            device, resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default),
            inputLayout, VS, PS, 1);
        BLK_ASSERT_VAR(res);

        inputLayout.Unload();

        return true;
    }

    void DebugRenderPass::Unload()
    {
        m_VertexBufferView.Unload();
        m_VertexBuffer.Unload();
        m_IndexBuffer.Unload();
        m_IndexBufferView.Unload();
        m_PSO.Unload();

        m_CurrentAngle = 0.0f;
    }

} // namespace Boolka