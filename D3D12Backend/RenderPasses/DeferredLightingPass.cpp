#include "stdafx.h"
#include "DeferredLightingPass.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceContainer.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "RenderSchedule/ResourceTracker.h"
#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/Device.h"

namespace Boolka
{
    struct Light
    {
        Vector4 viewPos_nearZ;
        Vector4 color_farZ;
    };

    struct Sun
    {
        Vector4 lightDirVS;
        Vector4 color;
        Matrix4x4 viewToShadow;
    };

    struct DeferredPass
    {
        Light lights[BLK_MAX_LIGHT_COUNT];
        Vector4u lightCount;
        Sun sun;
    };

    bool DeferredLightingPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& albedo = resourceContainer.GetTexture(ResourceContainer::Tex::GBufferAlbedo);
        Texture2D& normal = resourceContainer.GetTexture(ResourceContainer::Tex::GBufferNormal);
        Texture2D& depth = resourceContainer.GetTexture(ResourceContainer::Tex::GbufferDepth);
        Texture2D& lightBuffer = resourceContainer.GetTexture(ResourceContainer::Tex::LightBuffer);
        RenderTargetView& lightBufferRTV = resourceContainer.GetRTV(ResourceContainer::RTV::LightBuffer);
        DescriptorHeap& mainDescriptorHeap = resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        Buffer& frameConstantBuffer = resourceContainer.GetFlippableBuffer(frameIndex, ResourceContainer::FlipBuf::Frame);
        Buffer& passConstantBuffer = resourceContainer.GetFlippableBuffer(frameIndex, ResourceContainer::FlipBuf::DeferredLighting);
        UploadBuffer& passUploadBuffer = resourceContainer.GetFlippableUploadBuffer(frameIndex, ResourceContainer::FlipUploadBuf::DeferredLighting);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "DeferredLightingPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        // Temp lights position
        resourceTracker.Transition(passConstantBuffer, commandList, D3D12_RESOURCE_STATE_COPY_DEST);

        DeferredPass uploadData{};
        auto& lightContainer = frameContext.GetLightContainer();
        auto& lights = lightContainer.GetLights();

        for (size_t i = 0; i < lights.size(); ++i)
        {
            Vector3 viewPos = Vector4(lights[i].worldPos, 1.0f) * frameContext.GetViewMatrix();
            uploadData.lights[i].viewPos_nearZ = Vector4(viewPos, lights[i].nearZ);
            uploadData.lights[i].color_farZ = Vector4(lights[i].color, lights[i].farZ);
        }

        auto& sun = lightContainer.GetSun();
        uploadData.sun.lightDirVS = -(sun.lightDir * frameContext.GetViewMatrix());
        uploadData.sun.color = sun.color;
        uploadData.sun.viewToShadow = (frameContext.GetInvViewMatrix() * lightContainer.GetSunView() * lightContainer.GetSunProj() * Matrix4x4::GetUVToTexCoord()).Transpose();

        uploadData.lightCount = Vector4u(static_cast<uint>(lights.size()), 0, 0, 0);

        passUploadBuffer.Upload(&uploadData, sizeof(DeferredPass));
        commandList->CopyResource(passConstantBuffer.Get(), passUploadBuffer.Get());

        resourceTracker.Transition(passConstantBuffer, commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        resourceTracker.Transition(albedo, commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(normal, commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(depth, commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        resourceTracker.Transition(lightBuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

        for (size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
        {
            auto& shadowMap = resourceContainer.GetTexture(ResourceContainer::Tex::ShadowMapCube0 + lightIndex);
            resourceTracker.Transition(shadowMap, commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }

        commandList->OMSetRenderTargets(1, lightBufferRTV.GetCPUDescriptor(), FALSE, nullptr);
        ID3D12DescriptorHeap* descriptorHeaps[] = { mainDescriptorHeap.Get() };
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);
        commandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::RenderPassSRV), mainDescriptorHeap.GetGPUHandle(static_cast<UINT>(ResourceContainer::SRV::GBufferAlbedo)));

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

        commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer), frameConstantBuffer->GetGPUVirtualAddress());
        commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassConstantBuffer), passConstantBuffer->GetGPUVirtualAddress());

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetPipelineState(m_PSO.Get());

        commandList->DrawInstanced(3, 1, 0, 0);

        return true;
    }

    bool DeferredLightingPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool DeferredLightingPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();
        auto& defaultRootSig = resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);

        MemoryBlock PS = DebugFileReader::ReadFile("DeferredLightingPassPS.cso");
        MemoryBlock VS = DebugFileReader::ReadFile("FullScreenVS.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(0);

        // We are intentionally using empty input layout here to draw fullscreen quad without Vertex/Index buffers
        BLK_RENDER_DEBUG_ONLY(device.FilterMessage(D3D12_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT));
        bool res = m_PSO.Initialize(device, resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default), inputLayout, VS, PS, 1, false, false, D3D12_COMPARISON_FUNC_ALWAYS, false, DXGI_FORMAT_R16G16B16A16_FLOAT);
        BLK_RENDER_DEBUG_ONLY(device.RemoveLastMessageFilter());
        BLK_ASSERT(res);

        inputLayout.Unload();

        return true;
    }

    void DeferredLightingPass::Unload()
    {
        m_PSO.Unload();
    }

}