#include "stdafx.h"

#include "Debug3DRenderPass.h"

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
        // BLK_RENDER_PASS_START(Debug3DRenderPass);
        float deltaTime = renderContext.GetRenderFrameContext().GetDeltaTime();

        auto& resourceContainer = renderContext.GetRenderEngineContext().GetResourceContainer();

        UINT frameIndex = renderContext.GetRenderFrameContext().GetFrameIndex();
        Texture2D& backbuffer = resourceContainer.GetBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = resourceContainer.GetBackBufferRTV(frameIndex);
        DepthStencilView& gbufferDSV =
            resourceContainer.GetDSV(ResourceContainer::DSV::GbufferDepth);

        GraphicCommandListImpl& commandList =
            renderContext.GetRenderThreadContext().GetGraphicCommandList();

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE,
                                        gbufferDSV.GetCPUDescriptor());

        UINT height = renderContext.GetRenderEngineContext().GetBackbufferHeight();
        UINT width = renderContext.GetRenderEngineContext().GetBackbufferWidth();
        float aspectRatioCompensation = static_cast<float>(height) / width;

        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        UploadBuffer& currentUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::Frame);

        static const float rotationSpeed = BLK_FLOAT_PI / 8.0f;
        m_CurrentAngle += deltaTime * rotationSpeed;
        m_CurrentAngle = fmod(m_CurrentAngle, 2.0f * BLK_FLOAT_PI);

        Matrix4x4 view = {
            0.2f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f,
            0.0f, -0.05f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        };

        Matrix4x4 zRotationMatrix = Matrix4x4::GetRotationZ(m_CurrentAngle);

        static const float xRotationAngle = -0.2f;

        static const Matrix4x4 xRotationMatrix = Matrix4x4::GetRotationX(xRotationAngle);

        view = view * xRotationMatrix * zRotationMatrix;

        Matrix4x4 viewProj = view;
        viewProj[0] *= aspectRatioCompensation;

        currentUploadBuffer.Upload(viewProj.GetBuffer(), sizeof(viewProj));

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
        commandList->ClearDepthStencilView(*gbufferDSV.GetCPUDescriptor(), D3D12_CLEAR_FLAG_DEPTH,
                                           1.0f, 0, 0, nullptr);

        commandList->SetGraphicsRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer),
            frameConstantBuffer->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetIndexBuffer(m_IndexBufferView.GetView());

        D3D12_VERTEX_BUFFER_VIEW VertexBuffers[2] = {*m_VertexBufferView.GetView(),
                                                     *m_VertexInstanceBufferView.GetView()};

        commandList->IASetVertexBuffers(0, 2, VertexBuffers);
        commandList->SetPipelineState(m_PSO.Get());
        commandList->DrawIndexedInstanced(36, 5, 0, 0, 0);

        return true;
    }

    bool Debug3DRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool Debug3DRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        BLK_ASSERT(m_CurrentAngle == 0.0f);

        MemoryBlock PS = DebugFileReader::ReadFile("Debug3DPassPixelShader.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("Debug3DPassVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(6);
        inputLayout.SetEntry(0, {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
        inputLayout.SetEntry(1, {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
        inputLayout.SetEntry(2, {"WORLD_MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,
                                 D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1});
        inputLayout.SetEntry(3, {"WORLD_MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
                                 D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1});
        inputLayout.SetEntry(4, {"WORLD_MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32,
                                 D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1});
        inputLayout.SetEntry(5, {"WORLD_MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48,
                                 D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1});

        static const UINT64 vertexCount = 4 * 6;

        struct Vertex
        {
            float position[3];
            float color[3];
        } vertexData[vertexCount] = {
            {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}}, // -z
            {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
            {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
            {{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // -x
            {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}}, // -y
            {{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
            {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
            {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
            {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // +z
            {{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // +x
            {{1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
            {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}}, // +y
            {{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
            {{-1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
            {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        };

        static const UINT64 vertexSize = sizeof(Vertex);
        static const UINT64 vertexBufferSize = vertexSize * vertexCount;

        bool res = m_VertexBuffer.Initialize(device, vertexBufferSize);
        BLK_ASSERT_VAR(res);
        m_VertexBuffer.Upload(vertexData, vertexBufferSize);

        res = m_VertexBufferView.Initialize(m_VertexBuffer, vertexBufferSize, vertexSize);
        BLK_ASSERT_VAR(res);

        static const UINT64 instanceCount = 5;

        Matrix4x4 instanceData[instanceCount] = {Matrix4x4::GetTranslation(2.0f, 0.0f, 0.0f),
                                                 Matrix4x4::GetTranslation(0.0f, 2.0f, 0.0f),
                                                 Matrix4x4::GetTranslation(-2.0f, 0.0f, 0.0f),
                                                 Matrix4x4::GetTranslation(0.0f, -2.0f, 0.0f),
                                                 Matrix4x4::GetTranslation(0.0f, 0.0f, 2.0f) *
                                                     Matrix4x4::GetScale(0.5f) *
                                                     Matrix4x4::GetRotationZ(BLK_FLOAT_PI / 2.0f)};

        static const UINT64 instanceSize = sizeof(Matrix4x4);
        static const UINT64 instanceBufferSize = instanceSize * instanceCount;

        res = m_VertexInstanceBuffer.Initialize(device, instanceBufferSize);
        BLK_ASSERT_VAR(res);
        m_VertexInstanceBuffer.Upload(instanceData, instanceBufferSize);

        res = m_VertexInstanceBufferView.Initialize(m_VertexInstanceBuffer, instanceBufferSize,
                                                    instanceSize);
        BLK_ASSERT_VAR(res);

        static const UINT64 indexCount = 36;

        uint16_t indexData[indexCount] = {0,  1,  2,  2,  1,  3,  4,  5,  6,  6,  5,  7,
                                          8,  9,  10, 10, 9,  11, 12, 13, 14, 14, 13, 15,
                                          16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23};

        static const UINT64 indexSize = sizeof(uint16_t);
        static const UINT64 indexBufferSize = indexSize * indexCount;

        res = m_IndexBuffer.Initialize(device, indexBufferSize);
        BLK_ASSERT_VAR(res);
        m_IndexBuffer.Upload(indexData, indexBufferSize);

        res = m_IndexBufferView.Initialize(m_IndexBuffer, indexBufferSize, DXGI_FORMAT_R16_UINT);
        BLK_ASSERT_VAR(res);

        auto& resourceContainer = renderContext.GetRenderEngineContext().GetResourceContainer();

        res = m_PSO.Initialize(
            device, L"Debug3DRenderPass::m_PSO",
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default), inputLayout,
            VSParam{VS}, PSParam{PS}, DepthStencilParam{true, true}, DepthFormatParam{});
        BLK_ASSERT_VAR(res);

        inputLayout.Unload();

        return true;
    }

    void Debug3DRenderPass::Unload()
    {
        m_VertexBufferView.Unload();
        m_VertexBuffer.Unload();
        m_VertexInstanceBuffer.Unload();
        m_VertexInstanceBufferView.Unload();
        m_IndexBuffer.Unload();
        m_IndexBufferView.Unload();
        m_PSO.Unload();

        m_CurrentAngle = 0.0f;
    }

} // namespace Boolka