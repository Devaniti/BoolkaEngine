#include "stdafx.h"
#include "GBufferRenderPass.h"

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

    GBufferRenderPass::GBufferRenderPass()
    {
    }

    GBufferRenderPass::~GBufferRenderPass()
    {
    }

    bool GBufferRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        float deltaTime = renderContext.GetRenderFrameContext().GetDeltaTime();

        UINT frameIndex = renderContext.GetRenderFrameContext().GetFrameIndex();
        Texture2D& backbuffer = renderContext.GetRenderEngineContext().GetSwapchainBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = renderContext.GetRenderEngineContext().GetSwapchainRenderTargetView(frameIndex);

        GraphicCommandListImpl& commandList = renderContext.GetRenderThreadContext().GetGraphicCommandList();

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE, m_DSV.GetCPUDescriptor());
        ID3D12DescriptorHeap* descriptorHeaps[] = { renderContext.GetRenderEngineContext().GetScene().GetSRVDescriptorHeap().Get() };
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(6, renderContext.GetRenderEngineContext().GetScene().GetSRVDescriptorHeap().GetGPUHandle(0));

        UINT height = renderContext.GetRenderEngineContext().GetBackbufferHeight();
        UINT width = renderContext.GetRenderEngineContext().GetBackbufferWidth();
        float aspectRatio = static_cast<float>(width) / height;

        ConstantBuffer& currentConstantBuffer = m_ConstantBuffers[frameIndex];
        UploadBuffer& currentUploadBuffer = m_UploadBuffers[frameIndex];

        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
        bool res = m_Camera.Update(deltaTime, aspectRatio, viewMatrix, projectionMatrix);
        BLK_ASSERT(res);

        Matrix4x4 viewProjMatrix = viewMatrix * projectionMatrix;

        unsigned char* upload = static_cast<unsigned char*>(currentUploadBuffer.Map());
        memcpy(upload, viewProjMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));
        upload += sizeof(Matrix4x4);

        memcpy(upload, viewMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));

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
        commandList->ClearDepthStencilView(*m_DSV.GetCPUDescriptor(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        commandList->SetGraphicsRootConstantBufferView(0, currentConstantBuffer->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetIndexBuffer(m_IndexBufferView.GetView());

        commandList->IASetVertexBuffers(0, 1, m_VertexBufferView.GetView());
        commandList->SetPipelineState(m_PSO.Get());

        Scene& scene = renderContext.GetRenderEngineContext().GetScene();
        UINT indexes = scene.GetIndexBufferSize() / sizeof(uint32_t);
        commandList->DrawIndexedInstanced(indexes, 1, 0, 0, 0);

        ResourceTransition::Transition(currentConstantBuffer, commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);

        return true;
    }

    bool GBufferRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool GBufferRenderPass::Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        m_Camera.Initialize(0.0f, 0.0f, {0, 0, 1, 0});

        MemoryBlock PS = DebugFileReader::ReadFile("GBufferPassPixelShader.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("GBufferPassVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(4);
        inputLayout.SetEntry(0, { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(1, { "MATERIAL", 0, DXGI_FORMAT_R32_SINT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(2, { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(3, { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

        Scene& scene = renderContext.GetRenderEngineContext().GetScene();

        bool res = m_VertexBufferView.Initialize(scene.GetVertexBuffer(), scene.GetVertexBufferSize(), 9 * sizeof(float));
        BLK_ASSERT(res);

        res = m_IndexBufferView.Initialize(scene.GetIndexBuffer(), scene.GetIndexBufferSize(), DXGI_FORMAT_R32_UINT);
        BLK_ASSERT(res);

        res = m_PSO.Initialize(device, renderContext.GetRenderEngineContext().GetDefaultRootSig(), inputLayout, VS, PS, true);
        BLK_ASSERT(res);

        inputLayout.Unload();

        static const UINT64 floatSize = 4;
        static const UINT64 cbSize = CEIL_TO_POWER_OF_TWO(4 * 4 * floatSize, 256);

        INITIALIZE_ARRAY(m_ConstantBuffers, device, cbSize);
        INITIALIZE_ARRAY(m_UploadBuffers, device, cbSize);

        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE destHandle = renderContext.GetRenderEngineContext().GetMainDescriptorHeap().GetCPUHandle(i);
            m_ConstantBufferViews[i].Initialize(device, m_ConstantBuffers[i], destHandle, static_cast<UINT>(cbSize));
        }

        D3D12_CLEAR_VALUE dsvClearValue = {};
        dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        dsvClearValue.DepthStencil.Depth = 1.0f;

        res = m_DepthBuffer.Initialize(device, D3D12_HEAP_TYPE_DEFAULT, renderContext.GetRenderEngineContext().GetBackbufferWidth(), 
            renderContext.GetRenderEngineContext().GetBackbufferHeight(), 1, DXGI_FORMAT_D32_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &dsvClearValue, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        BLK_ASSERT(res);
        
        res = m_DSV.Initialize(device, m_DepthBuffer, DXGI_FORMAT_D32_FLOAT, renderContext.GetRenderEngineContext().GetDSVDescriptorHeap().GetCPUHandle(0));
        BLK_ASSERT(res);

        return true;
    }

    void GBufferRenderPass::Unload()
    {
        UNLOAD_ARRAY(m_ConstantBuffers);
        UNLOAD_ARRAY(m_UploadBuffers);
        UNLOAD_ARRAY(m_ConstantBufferViews);

        m_DSV.Unload();
        m_DepthBuffer.Unload();
        m_VertexBufferView.Unload();
        m_IndexBufferView.Unload();
        m_PSO.Unload();
        m_Camera.Unload();
    }

}