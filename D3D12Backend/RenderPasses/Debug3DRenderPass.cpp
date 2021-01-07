#include "stdafx.h"
#include "Debug3DRenderPass.h"

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

    Debug3DRenderPass::Debug3DRenderPass()
        : m_CurrentAngle(0.0f)
    {
    }

    Debug3DRenderPass::~Debug3DRenderPass()
    {
        BLK_ASSERT(m_CurrentAngle == 0.0f);
    }

    bool Debug3DRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        float deltaTime = renderContext.GetRenderFrameContext().GetDeltaTime();

        UINT frameIndex = renderContext.GetRenderFrameContext().GetFrameIndex();
        Texture2D& backbuffer = renderContext.GetRenderEngineContext().GetSwapchainBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = renderContext.GetRenderEngineContext().GetSwapchainRenderTargetView(frameIndex);

        GraphicCommandListImpl& commandList = renderContext.GetRenderThreadContext().GetGraphicCommandList();

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE, m_DSV.GetCPUDescriptor());

        UINT height = renderContext.GetRenderEngineContext().GetBackbufferHeight();
        UINT width = renderContext.GetRenderEngineContext().GetBackbufferWidth();
        float aspectRatioCompensation = static_cast<float>(height) / width;

        Buffer& currentConstantBuffer = m_ConstantBuffers[frameIndex];
        UploadBuffer& currentUploadBuffer = m_UploadBuffers[frameIndex];

        static const float rotationSpeed = FLOAT_PI / 8.0f;
        m_CurrentAngle += deltaTime * rotationSpeed;
        m_CurrentAngle = fmod(m_CurrentAngle, 2.0f * FLOAT_PI);

        Matrix4x4 view = 
        {
            0.2f,  0.0f, 0.0f, 0.0f,
            0.0f,  0.0f, 0.2f, 0.0f,
            0.0f,-0.05f, 0.0f, 0.5f,
            0.0f,  0.0f, 0.0f, 1.0f,
        };

        Matrix4x4 zRotationMatrix = Matrix4x4::GetRotationZ(m_CurrentAngle);

        static const float xRotationAngle = -0.2f;

        static const Matrix4x4 xRotationMatrix = Matrix4x4::GetRotationX(xRotationAngle);

        view = view * xRotationMatrix* zRotationMatrix;

        Matrix4x4 viewProj = view;
        viewProj[0] *= aspectRatioCompensation;

        currentUploadBuffer.Upload(viewProj.GetBuffer(), sizeof(viewProj));

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

        D3D12_VERTEX_BUFFER_VIEW VertexBuffers[2] = { *m_VertexBufferView.GetView(),  *m_VertexInstanceBufferView.GetView() };

        commandList->IASetVertexBuffers(0, 2, VertexBuffers);
        commandList->SetPipelineState(m_PSO.Get());
        commandList->DrawIndexedInstanced(36, 5, 0, 0, 0);

        ResourceTransition::Transition(currentConstantBuffer, commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);

        return true;
    }

    bool Debug3DRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool Debug3DRenderPass::Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_ASSERT(m_CurrentAngle == 0.0f);

        MemoryBlock PS = DebugFileReader::ReadFile("Debug3DPassPixelShader.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("Debug3DPassVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(6);
        inputLayout.SetEntry(0, { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(1, { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
        inputLayout.SetEntry(2, { "WORLD_MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        inputLayout.SetEntry(3, { "WORLD_MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        inputLayout.SetEntry(4, { "WORLD_MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });
        inputLayout.SetEntry(5, { "WORLD_MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 });

        static const UINT64 vertexCount = 4 * 6;

        struct Vertex
        {
            float position[3];
            float color[3];
        } vertexData[vertexCount] =
        {
            { {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f} }, // -z
            { { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f} },
            { {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f} },
            { { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f} },
            { {-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} }, // -x
            { {-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
            { {-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },
            { {-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },
            { {-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} }, // -y
            { {-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f} },
            { { 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} },
            { { 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f} },
            { {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} }, // +z
            { {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },
            { { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },
            { { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },
            { { 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} }, // +x
            { { 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },
            { { 1.0f,  1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
            { { 1.0f,  1.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },
            { {-1.0f,  1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} }, // +y
            { { 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} },
            { {-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f, 0.0f} },
            { { 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f, 0.0f} },
        };

        static const UINT64 vertexSize = sizeof(Vertex);
        static const UINT64 vertexBufferSize = vertexSize * vertexCount;

        bool res = m_VertexBuffer.Initialize(device, vertexBufferSize);
        BLK_ASSERT(res);
        m_VertexBuffer.Upload(vertexData, vertexBufferSize);

        res = m_VertexBufferView.Initialize(m_VertexBuffer, vertexBufferSize, vertexSize);
        BLK_ASSERT(res);

        static const UINT64 instanceCount = 5;

        Matrix4x4 instanceData[instanceCount] = 
        {
            Matrix4x4::GetTranslation( 2.0f,  0.0f, 0.0f),
            Matrix4x4::GetTranslation( 0.0f,  2.0f, 0.0f),
            Matrix4x4::GetTranslation(-2.0f,  0.0f, 0.0f),
            Matrix4x4::GetTranslation( 0.0f, -2.0f, 0.0f),
            Matrix4x4::GetTranslation(0.0f, 0.0f, 2.0f) * Matrix4x4::GetScale(0.5f) * Matrix4x4::GetRotationZ(FLOAT_PI / 2.0f)
        };

        static const UINT64 instanceSize = sizeof(Matrix4x4);
        static const UINT64 instanceBufferSize = instanceSize * instanceCount;

        res = m_VertexInstanceBuffer.Initialize(device, instanceBufferSize);
        BLK_ASSERT(res);
        m_VertexInstanceBuffer.Upload(instanceData, instanceBufferSize);

        res = m_VertexInstanceBufferView.Initialize(m_VertexInstanceBuffer, instanceBufferSize, instanceSize);
        BLK_ASSERT(res);

        static const UINT64 indexCount = 36;

        uint16_t indexData[indexCount] = 
        { 
            0, 1, 2, 2, 1, 3, 
            4, 5, 6, 6, 5, 7,
            8, 9, 10, 10, 9, 11,
            12, 13, 14, 14, 13, 15,
            16, 17, 18, 18, 17, 19,
            20, 21, 22, 22, 21, 23
        };

        static const UINT64 indexSize = sizeof(uint16_t);
        static const UINT64 indexBufferSize = indexSize * indexCount;

        res = m_IndexBuffer.Initialize(device, indexBufferSize);
        BLK_ASSERT(res);
        m_IndexBuffer.Upload(indexData, indexBufferSize);

        res = m_IndexBufferView.Initialize(m_IndexBuffer, indexBufferSize, DXGI_FORMAT_R16_UINT);
        BLK_ASSERT(res);

        res = m_PSO.Initialize(device, renderContext.GetRenderEngineContext().GetDefaultRootSig(), inputLayout, VS, PS, 1, true);
        BLK_ASSERT(res);

        inputLayout.Unload();

        static const UINT64 floatSize = 4;
        static const UINT64 cbSize = CEIL_TO_POWER_OF_TWO(4 * 4 * floatSize, 256);

        INITIALIZE_ARRAY(m_ConstantBuffers, device, cbSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
        INITIALIZE_ARRAY(m_UploadBuffers, device, cbSize);

        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE destHandle = renderContext.GetRenderEngineContext().GetMainDescriptorHeap().GetCPUHandle(i);
            m_ConstantBufferViews[i].Initialize(device, m_ConstantBuffers[i], destHandle, static_cast<UINT>(cbSize));
        }

        D3D12_CLEAR_VALUE dsvClearValue = {};
        dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        dsvClearValue.DepthStencil.Depth = 1.0f;

        res = m_DepthBuffer.Initialize(device, D3D12_HEAP_TYPE_DEFAULT, 
            renderContext.GetRenderEngineContext().GetBackbufferWidth(), 
            renderContext.GetRenderEngineContext().GetBackbufferHeight(), 
            1, DXGI_FORMAT_D32_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 
            &dsvClearValue, D3D12_RESOURCE_STATE_DEPTH_WRITE);

        BLK_ASSERT(res);
        
        res = m_DSV.Initialize(device, m_DepthBuffer, DXGI_FORMAT_D32_FLOAT, renderContext.GetRenderEngineContext().GetDSVDescriptorHeap().GetCPUHandle(0));
        BLK_ASSERT(res);

        return true;
    }

    void Debug3DRenderPass::Unload()
    {
        UNLOAD_ARRAY(m_ConstantBuffers);
        UNLOAD_ARRAY(m_UploadBuffers);
        UNLOAD_ARRAY(m_ConstantBufferViews);

        m_DSV.Unload();
        m_DepthBuffer.Unload();
        m_VertexBufferView.Unload();
        m_VertexBuffer.Unload();
        m_VertexInstanceBuffer.Unload();
        m_VertexInstanceBufferView.Unload();
        m_IndexBuffer.Unload();
        m_IndexBufferView.Unload();
        m_PSO.Unload();

        m_CurrentAngle = 0.0f;
    }

}