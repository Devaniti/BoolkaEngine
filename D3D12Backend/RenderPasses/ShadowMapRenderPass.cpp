#include "stdafx.h"

#include "ShadowMapRenderPass.h"

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/InputLayout.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "BoolkaCommon/DebugHelpers/DebugFileReader.h"
#include "BoolkaCommon/Structures/Frustum.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool ShadowMapRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(ShadowMapRenderPass);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        auto& batchManager = engineContext.GetScene().GetBatchManager();

        auto& lightContainer = frameContext.GetLightContainer();
        auto& lights = lightContainer.GetLights();

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

        engineContext.BindSceneResourcesGraphic(commandList);
        commandList->SetPipelineState(
            engineContext.GetPSOContainer().GetPSO(PSOContainer::GraphicPSO::ShadowMap).Get());

        // Transition all shadowmaps to DEPTH_WRITE
        for (size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
        {
            auto& shadowMap =
                resourceContainer.GetTexture(ResourceContainer::Tex2D::ShadowMapCube0 + lightIndex);
            resourceTracker.Transition(shadowMap, commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        }
        {
            auto& shadowMap = resourceContainer.GetTexture(ResourceContainer::Tex2D::ShadowMapSun);
            resourceTracker.Transition(shadowMap, commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        }

        // Clear shadowmaps
        for (size_t resourceIndex = 0; resourceIndex < lights.size() * BLK_TEXCUBE_FACE_COUNT;
             ++resourceIndex)
        {
            auto& shadowMapDSV =
                resourceContainer.GetDSV(ResourceContainer::DSV::ShadowMapLight0 + resourceIndex);
            commandList->ClearDepthStencilView(*shadowMapDSV.GetCPUDescriptor(),
                                               D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        }
        {
            auto& sunShadowMapDSV = resourceContainer.GetDSV(ResourceContainer::DSV::ShadowMapSun);
            commandList->ClearDepthStencilView(*sunShadowMapDSV.GetCPUDescriptor(),
                                               D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        }

        // Render shadowmaps
        for (size_t lightIndex = 0; lightIndex < lights.size(); ++lightIndex)
        {
            for (size_t faceIndex = 0; faceIndex < BLK_TEXCUBE_FACE_COUNT; ++faceIndex)
            {
                size_t resourceIndex = lightIndex * BLK_TEXCUBE_FACE_COUNT + faceIndex;

                auto& shadowMapDSV = resourceContainer.GetDSV(
                    ResourceContainer::DSV::ShadowMapLight0 + resourceIndex);

                commandList->OMSetRenderTargets(0, nullptr, FALSE, shadowMapDSV.GetCPUDescriptor());
                commandList->SetGraphicsRoot32BitConstant(
                    static_cast<UINT>(
                        ResourceContainer::DefaultRootSigBindPoints::PassRootConstant),
                    static_cast<UINT>(BatchManager::ViewType::ShadowMapLight0 + resourceIndex), 0);

                batchManager.Render(commandList, renderContext,
                                    BatchManager::BatchType::ShadowMapLight0 + resourceIndex);
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

            auto& shadowMapDSV = resourceContainer.GetDSV(ResourceContainer::DSV::ShadowMapSun);

            commandList->OMSetRenderTargets(0, nullptr, FALSE, shadowMapDSV.GetCPUDescriptor());
            commandList->SetGraphicsRoot32BitConstant(
                static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassRootConstant),
                static_cast<UINT>(BatchManager::ViewType::ShadowMapSun), 0);

            engineContext.GetScene().GetBatchManager().Render(
                commandList, renderContext, BatchManager::BatchType::ShadowMapSun);
        }

        return true;
    }

    bool ShadowMapRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ShadowMapRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        return true;
    }

    void ShadowMapRenderPass::Unload()
    {
    }

} // namespace Boolka
