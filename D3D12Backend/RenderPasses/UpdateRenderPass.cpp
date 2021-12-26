#include "stdafx.h"

#include "UpdateRenderPass.h"

#include "APIWrappers/Resources/ResourceTransition.h"
#include "BoolkaCommon/Structures/Frustum.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool UpdateRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        BLK_INITIALIZE_ARRAY(m_ReadbackBuffers, device, 256 * sizeof(uint));
        return true;
    }

    void UpdateRenderPass::Unload()
    {
        BLK_UNLOAD_ARRAY(m_ReadbackBuffers);
    }

    bool UpdateRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(UpdateRenderPass);
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        UploadFrameConstantBuffer(renderContext, resourceTracker);
        UploadLightingConstantBuffer(renderContext, resourceTracker);
        UploadCullingConstantBuffer(renderContext, resourceTracker);
        ReadbackDebugMarkersBuffer(renderContext, resourceTracker);

        return true;
    }

    bool UpdateRenderPass::PrepareRendering()
    {
        return true;
    }

    void UpdateRenderPass::UploadFrameConstantBuffer(RenderContext& renderContext,
                                                     ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        Buffer& perFrameCbuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        UploadBuffer& currentUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::Frame);
        ResourceTransition::Transition(commandList, perFrameCbuffer,
                                       D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                                       D3D12_RESOURCE_STATE_COPY_DEST);

        HLSLShared::FrameConstantBuffer frameCbufferData{
            .viewProjMatrix = frameContext.GetViewProjMatrix().Transpose(),
            .viewMatrix = frameContext.GetViewMatrix().Transpose(),
            .projMatrix = frameContext.GetProjMatrix().Transpose(),
            .invViewProjMatrix = frameContext.GetInvViewProjMatrix().Transpose(),
            .invViewMatrix = frameContext.GetInvViewMatrix().Transpose(),
            .invProjMatrix = frameContext.GetInvProjMatrix().Transpose(),
            .mainViewFrustum = Frustum(frameContext.GetViewProjMatrix()),
            .eyeRayCoeficients = {},
            .cameraWorldPos = frameContext.GetCameraPos(),
            .backbufferResolution =
                Vector2(static_cast<float>(engineContext.GetBackbufferWidth()),
                        static_cast<float>(engineContext.GetBackbufferHeight())),
            .invBackBufferResolution = Vector2(1.0f / engineContext.GetBackbufferWidth(),
                                               1.0f / engineContext.GetBackbufferHeight())};
        memcpy(&frameCbufferData.eyeRayCoeficients, frameContext.GetEyeRayCoeficients(),
               sizeof(frameCbufferData.eyeRayCoeficients));

        currentUploadBuffer.Upload(&frameCbufferData, sizeof(frameCbufferData));

        commandList->CopyResource(perFrameCbuffer.Get(), currentUploadBuffer.Get());

        ResourceTransition::Transition(commandList, perFrameCbuffer, D3D12_RESOURCE_STATE_COPY_DEST,
                                       D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    void UpdateRenderPass::UploadLightingConstantBuffer(RenderContext& renderContext,
                                                        ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        Buffer& lightingCbuffer =
            resourceContainer.GetBuffer(ResourceContainer::Buf::DeferredLighting);
        UploadBuffer& lightingCbufferUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::DeferredLighting);

        resourceTracker.Transition(lightingCbuffer, commandList, D3D12_RESOURCE_STATE_COPY_DEST);

        HLSLShared::LightingDataConstantBuffer lightingCbufferData{};
        auto& lightContainer = frameContext.GetLightContainer();
        auto& lights = lightContainer.GetLights();

        for (size_t i = 0; i < lights.size(); ++i)
        {
            Vector3 viewPos = Vector4(lights[i].worldPos, 1.0f) * frameContext.GetViewMatrix();
            lightingCbufferData.lights[i].viewPos = viewPos;
            lightingCbufferData.lights[i].nearZ = lights[i].nearZ;
            lightingCbufferData.lights[i].color = lights[i].color;
            lightingCbufferData.lights[i].farZ = lights[i].farZ;
        }

        auto& sun = lightContainer.GetSun();
        lightingCbufferData.sun.lightDirVS = -(sun.lightDir * frameContext.GetViewMatrix());
        lightingCbufferData.sun.color = sun.color;
        lightingCbufferData.sun.viewToShadow =
            (frameContext.GetInvViewMatrix() * lightContainer.GetSunView() *
             lightContainer.GetSunProj() * Matrix4x4::GetUVToTexCoord())
                .Transpose();

        lightingCbufferData.lightCount = Vector4u(static_cast<uint>(lights.size()), 0, 0, 0);
        lightingCbufferUploadBuffer.Upload(&lightingCbufferData, sizeof(lightingCbufferData));
        commandList->CopyResource(lightingCbuffer.Get(), lightingCbufferUploadBuffer.Get());

        resourceTracker.Transition(lightingCbuffer, commandList,
                                   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    void UpdateRenderPass::UploadCullingConstantBuffer(RenderContext& renderContext,
                                                       ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        Buffer& lightingCbuffer =
            resourceContainer.GetBuffer(ResourceContainer::Buf::DeferredLighting);

        Buffer& cullingCbuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCullingCB);
        UploadBuffer& cullingCbufferUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::GPUCulling);

        resourceTracker.Transition(cullingCbuffer, commandList, D3D12_RESOURCE_STATE_COPY_DEST);

        HLSLShared::CullingDataConstantBuffer cullingCbufferData{};
        cullingCbufferData.objectCount =
            Vector4u(engineContext.GetScene().GetOpaqueObjectCount(), 0, 0, 0);

        cullingCbufferData.views[static_cast<size_t>(BatchManager::ViewType::MainView)] =
            Frustum(frameContext.GetViewProjMatrix());
        cullingCbufferData.viewProjMatrix[static_cast<size_t>(BatchManager::ViewType::MainView)] =
            frameContext.GetViewProjMatrix().Transpose();
        cullingCbufferData.cameraPos[static_cast<size_t>(BatchManager::ViewType::MainView)] =
            frameContext.GetCameraPos();

        const auto& lightContainer = frameContext.GetLightContainer();
        cullingCbufferData.views[static_cast<size_t>(BatchManager::ViewType::ShadowMapSun)] =
            Frustum(lightContainer.GetSunViewProj());
        cullingCbufferData
            .viewProjMatrix[static_cast<size_t>(BatchManager::ViewType::ShadowMapSun)] =
            lightContainer.GetSunViewProj().Transpose();
        frameContext.GetCameraPos() - lightContainer.GetSun().lightDir * 1000000.0f;

        size_t lightCount = lightContainer.GetLights().size();
        const auto& lightViewProjMatricies = lightContainer.GetViewProjMatrices();
        BLK_ASSERT(lightViewProjMatricies.size() == lightCount);

        for (size_t i = 0; i < lightCount; ++i)
            for (size_t j = 0; j < BLK_TEXCUBE_FACE_COUNT; ++j)
            {
                cullingCbufferData
                    .views[static_cast<size_t>(BatchManager::ViewType::ShadowMapLight0) +
                           i * BLK_TEXCUBE_FACE_COUNT + j] = Frustum(lightViewProjMatricies[i][j]);

                cullingCbufferData
                    .cameraPos[static_cast<size_t>(BatchManager::ViewType::ShadowMapLight0) +
                               i * BLK_TEXCUBE_FACE_COUNT + j] =
                    lightContainer.GetLights()[i].worldPos;
            }

        for (size_t i = 0; i < lightCount; ++i)
            for (size_t j = 0; j < BLK_TEXCUBE_FACE_COUNT; ++j)
            {
                cullingCbufferData
                    .viewProjMatrix[static_cast<size_t>(BatchManager::ViewType::ShadowMapLight0) +
                                    i * BLK_TEXCUBE_FACE_COUNT + j] =
                    lightViewProjMatricies[i][j].Transpose();
            }

        cullingCbufferUploadBuffer.Upload(&cullingCbufferData, sizeof(cullingCbufferData));
        commandList->CopyResource(cullingCbuffer.Get(), cullingCbufferUploadBuffer.Get());

        resourceTracker.Transition(cullingCbuffer, commandList,
                                   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }

    void UpdateRenderPass::ReadbackDebugMarkersBuffer(RenderContext& renderContext,
                                                      ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();
        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();
        UINT frameIndex = frameContext.GetFrameIndex();

        Buffer& markersBuf = resourceContainer.GetBuffer(ResourceContainer::Buf::DebugMarkers);
        resourceTracker.Transition(markersBuf, commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);

        FrameStats& frameStats = frameContext.GetFrameStats();
        m_ReadbackBuffers[frameIndex].Readback(frameStats.gpuDebugMarkers,
                                               sizeof(frameStats.gpuDebugMarkers));
        commandList->CopyResource(m_ReadbackBuffers[frameIndex].Get(), markersBuf.Get());

        resourceTracker.Transition(markersBuf, commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        ID3D12DescriptorHeap* descriptorHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap).Get();
        commandList->SetDescriptorHeaps(1, &descriptorHeap);

        const UINT clearValues[4] = {};
        commandList->ClearUnorderedAccessViewUint(
            resourceContainer.GetGPUDescriptor(ResourceContainer::Buf::DebugMarkers),
            resourceContainer.GetCPUVisibleCPUDescriptor(ResourceContainer::Buf::DebugMarkers),
            markersBuf.Get(), clearValues, 0, nullptr);
    }

} // namespace Boolka
