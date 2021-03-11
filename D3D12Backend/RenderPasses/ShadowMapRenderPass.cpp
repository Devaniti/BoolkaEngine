#include "stdafx.h"

#include "ShadowMapRenderPass.h"

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
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

    bool ShadowMapRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "ShadowMapRenderPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        auto& passConstantBuffer =
            resourceContainer.GetFlippableBuffer(frameIndex, ResourceContainer::FlipBuf::ShadowMap);
        auto& passUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::ShadowMap);

        resourceTracker.Transition(passConstantBuffer, commandList, D3D12_RESOURCE_STATE_COPY_DEST);

        Matrix4x4 uploadData[BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT + 1];

        // Point lights
        auto& lightContainer = frameContext.GetLightContainer();
        auto& lights = lightContainer.GetLights();
        for (size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
        {
            auto& shadowMap =
                resourceContainer.GetTexture(ResourceContainer::Tex::ShadowMapCube0 + lightIndex);
            resourceTracker.Transition(shadowMap, commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);

            for (size_t faceIndex = 0; faceIndex < BLK_TEXCUBE_FACE_COUNT; ++faceIndex)
            {
                size_t resourceIndex = lightIndex * BLK_TEXCUBE_FACE_COUNT + faceIndex;
                uploadData[resourceIndex] =
                    lightContainer.GetViewProjMatrices()[lightIndex][faceIndex].Transpose();
            }
        }

        // Sun light
        uploadData[BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT] =
            (lightContainer.GetSunView() * lightContainer.GetSunProj()).Transpose();

        passUploadBuffer.Upload(&uploadData, sizeof(uploadData));
        commandList->CopyResource(passConstantBuffer.Get(), passUploadBuffer.Get());

        resourceTracker.Transition(passConstantBuffer, commandList,
                                   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        UINT height = BLK_LIGHT_SHADOWMAP_SIZE;
        UINT width = BLK_LIGHT_SHADOWMAP_SIZE;

        D3D12_VIEWPORT viewportDesc = {};
        viewportDesc.Width = static_cast<float>(width);
        viewportDesc.Height = static_cast<float>(height);
        viewportDesc.MaxDepth = 1.0f;

        commandList->RSSetViewports(1, &viewportDesc);

        D3D12_RECT scissorRect = {};
        scissorRect.right = width;
        scissorRect.bottom = height;

        commandList->RSSetScissorRects(1, &scissorRect);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetIndexBuffer(engineContext.GetScene().GetIndexBufferView().GetView());

        commandList->IASetVertexBuffers(0, 1,
                                        engineContext.GetScene().GetVertexBufferView().GetView());
        commandList->SetPipelineState(m_PSO.Get());

        commandList->SetGraphicsRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassConstantBuffer),
            passConstantBuffer->GetGPUVirtualAddress());

        // Point lights
        for (size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
        {
            auto& shadowMap =
                resourceContainer.GetTexture(ResourceContainer::Tex::ShadowMapCube0 + lightIndex);
            resourceTracker.Transition(shadowMap, commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);

            for (size_t faceIndex = 0; faceIndex < BLK_TEXCUBE_FACE_COUNT; ++faceIndex)
            {
                size_t resourceIndex = lightIndex * BLK_TEXCUBE_FACE_COUNT + faceIndex;

                auto& shadowMapDSV =
                    resourceContainer.GetDSV(ResourceContainer::DSV::ShadowMapLight0 +
                                             lightIndex * BLK_TEXCUBE_FACE_COUNT + faceIndex);

                commandList->ClearDepthStencilView(*shadowMapDSV.GetCPUDescriptor(),
                                                   D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
                commandList->OMSetRenderTargets(0, nullptr, FALSE, shadowMapDSV.GetCPUDescriptor());
                commandList->SetGraphicsRoot32BitConstant(
                    static_cast<UINT>(
                        ResourceContainer::DefaultRootSigBindPoints::PassRootConstant),
                    static_cast<UINT>(resourceIndex), 0);

                engineContext.GetScene().GetBatchManager().Render(
                    commandList, BatchManager::BatchType::ShadowMapLight0 + resourceIndex);
            }
        }
        // Sun light
        {
            UINT height = BLK_SUN_SHADOWMAP_SIZE;
            UINT width = BLK_SUN_SHADOWMAP_SIZE;

            D3D12_VIEWPORT viewportDesc = {};
            viewportDesc.Width = static_cast<float>(width);
            viewportDesc.Height = static_cast<float>(height);
            viewportDesc.MaxDepth = 1.0f;

            commandList->RSSetViewports(1, &viewportDesc);

            D3D12_RECT scissorRect = {};
            scissorRect.right = width;
            scissorRect.bottom = height;

            commandList->RSSetScissorRects(1, &scissorRect);

            auto& shadowMap = resourceContainer.GetTexture(ResourceContainer::Tex::ShadowMapSun);
            resourceTracker.Transition(shadowMap, commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
            auto& shadowMapDSV = resourceContainer.GetDSV(ResourceContainer::DSV::ShadowMapSun);

            commandList->ClearDepthStencilView(*shadowMapDSV.GetCPUDescriptor(),
                                               D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
            commandList->OMSetRenderTargets(0, nullptr, FALSE, shadowMapDSV.GetCPUDescriptor());
            commandList->SetGraphicsRoot32BitConstant(
                static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassRootConstant),
                static_cast<UINT>(BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT), 0);

            engineContext.GetScene().GetBatchManager().Render(
                commandList, BatchManager::BatchType::ShadowMapSun);
        }

        return true;
    }

    bool ShadowMapRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ShadowMapRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();
        auto& defaultRootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);

        MemoryBlock PS = {};
        MemoryBlock VS = DebugFileReader::ReadFile("ShadowMapVertexShader.cso");
        InputLayout inputLayout;
        inputLayout.Initialize(1);
        inputLayout.SetEntry(0, {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                                 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});

        bool res = m_PSO.Initialize(device, defaultRootSig, inputLayout, VS, PS, 0, true);
        BLK_ASSERT(res);

        inputLayout.Unload();

        return true;
    }

    void ShadowMapRenderPass::Unload()
    {
        m_PSO.Unload();
    }

} // namespace Boolka
