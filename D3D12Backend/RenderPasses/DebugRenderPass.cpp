#include "stdafx.h"
#include "DebugRenderPass.h"

#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "APIWrappers/InputLayout.h"
#include "RenderSchedule/ResourceTracker.h"
#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/ResourceTransition.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderThreadContext.h"

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
        float deltaTime = renderContext.GetRenderFrameContext().GetDeltaTime();

        UINT frameIndex = renderContext.GetRenderFrameContext().GetFrameIndex();
        Texture2D& backbuffer = renderContext.GetRenderEngineContext().GetSwapchainBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = renderContext.GetRenderEngineContext().GetSwapchainRenderTargetView(frameIndex);

        GraphicCommandListImpl& commandList = renderContext.GetRenderThreadContext().GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "DebugRenderPass");

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE, nullptr);

        UINT height = renderContext.GetRenderEngineContext().GetBackbufferHeight();
        UINT width = renderContext.GetRenderEngineContext().GetBackbufferWidth();
        float aspectRatioCompensation = static_cast<float>(height) / width;

        Buffer& currentConstantBuffer = m_ConstantBuffers[frameIndex];
        UploadBuffer& currentUploadBuffer = m_UploadBuffers[frameIndex];

        static const float rotationSpeed = FLOAT_PI / 2.0f;
        m_CurrentAngle += deltaTime * rotationSpeed;
        m_CurrentAngle = fmod(m_CurrentAngle, 2.0f * FLOAT_PI);

        float* upload = static_cast<float*>(currentUploadBuffer.Map());
        upload[0] = aspectRatioCompensation * cos(m_CurrentAngle);
        upload[1] = -sin(m_CurrentAngle);
        upload[2] = aspectRatioCompensation * sin(m_CurrentAngle);
        upload[3] = cos(m_CurrentAngle);
        currentUploadBuffer.Unmap();

        commandList->CopyResource(currentConstantBuffer.Get(), currentUploadBuffer.Get());

        ResourceTransition::Transition(currentConstantBuffer, commandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        D3D12_VIEWPORT viewportDesc = {};
        viewportDesc.Width = static_cast<float>(width);
        viewportDesc.Height = static_cast<float>(height);
        viewportDesc.MaxDepth = 1.0f;

        commandList->RSSetViewports(1, &viewportDesc);

        D3D12_RECT scissorRect = {};
        scissorRect.right = width;
        scissorRect.bottom = height;

        commandList->RSSetScissorRects(1, &scissorRect);

        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        commandList->ClearRenderTargetView(*backbufferRTV.GetCPUDescriptor(), clearColor, 0, nullptr);

        commandList->SetGraphicsRootConstantBufferView(0, currentConstantBuffer->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetIndexBuffer(m_IndexBufferView.GetView());
        commandList->IASetVertexBuffers(0, 1, m_VertexBufferView.GetView());
        commandList->SetPipelineState(m_PSO.Get());
        commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);

        ResourceTransition::Transition(currentConstantBuffer, commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);

        return true;
    }

    bool DebugRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool DebugRenderPass::Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_ASSERT(m_CurrentAngle == 0.0f);

        MemoryBlock PS = DebugFileReader::ReadFile("DebugPassPixelShader.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("DebugPassVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(2);
        inputLayout.SetEntry(0, { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(1, { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

        static const UINT64 vertexCount = 3;

        static const float a = 0.8f;

        struct Vertex
        {
            float position[2];
            float color[3];
        } vertexData[vertexCount] =
        {
            { {-sqrt(3.0f) / 2.0f * a, -a / 2.0f}, {1.0f, 0.0f, 0.0f} },
            { {sqrt(3.0f) / 2.0f * a, -a / 2.0f}, {0.0f, 1.0f, 0.0f} },
            { {0.0f, a}, {0.0f, 0.0f, 1.0f} }
        };

        static const UINT64 vertexSize = sizeof(Vertex);
        static const UINT64 vertexBufferSize = vertexSize * vertexCount;

        bool res = m_VertexBuffer.Initialize(device, vertexBufferSize);
        BLK_ASSERT(res);
        m_VertexBuffer.Upload(vertexData, vertexBufferSize);

        res = m_VertexBufferView.Initialize(m_VertexBuffer, vertexBufferSize, vertexSize);
        BLK_ASSERT(res);

        static const UINT64 indexCount = 3;

        uint16_t indexData[indexCount] = { 0, 1, 2 };

        static const UINT64 indexSize = sizeof(uint16_t);
        static const UINT64 indexBufferSize = indexSize * indexCount;

        res = m_IndexBuffer.Initialize(device, indexBufferSize);
        BLK_ASSERT(res);
        m_IndexBuffer.Upload(indexData, indexBufferSize);

        res = m_IndexBufferView.Initialize(m_IndexBuffer, indexBufferSize, DXGI_FORMAT_R16_UINT);
        BLK_ASSERT(res);

        res = m_PSO.Initialize(device, renderContext.GetRenderEngineContext().GetDefaultRootSig(), inputLayout, VS, PS, 1);
        BLK_ASSERT(res);

        inputLayout.Unload();

        static const UINT64 floatSize = 4;
        static const UINT64 cbSize = BLK_CEIL_TO_POWER_OF_TWO(4 * floatSize, 256);

        BLK_INITIALIZE_ARRAY(m_ConstantBuffers, device, cbSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
        BLK_INITIALIZE_ARRAY(m_UploadBuffers, device, cbSize);

        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE destHandle = renderContext.GetRenderEngineContext().GetMainDescriptorHeap().GetCPUHandle(i);
            m_ConstantBufferViews[i].Initialize(device, m_ConstantBuffers[i], destHandle, static_cast<UINT>(cbSize));
        }

        return true;
    }

    void DebugRenderPass::Unload()
    {
        BLK_UNLOAD_ARRAY(m_ConstantBuffers);
        BLK_UNLOAD_ARRAY(m_UploadBuffers);
        BLK_UNLOAD_ARRAY(m_ConstantBufferViews);

        m_VertexBufferView.Unload();
        m_VertexBuffer.Unload();
        m_IndexBuffer.Unload();
        m_IndexBufferView.Unload();
        m_PSO.Unload();

        m_CurrentAngle = 0.0f;
    }

}