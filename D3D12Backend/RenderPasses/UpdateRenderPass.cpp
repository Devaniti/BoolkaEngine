#include "stdafx.h"

#include "UpdateRenderPass.h"

#include "APIWrappers/Resources/ResourceTransition.h"
#include "BoolkaCommon/Structures/Frustum.h"
#include "Containers/HLSLSharedStructures.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool UpdateRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        return true;
    }

    void UpdateRenderPass::Unload()
    {
    }

    bool UpdateRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(UpdateRenderPass);
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Buffer& perFrameCbuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        UploadBuffer& currentUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::Frame);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();


        ResourceTransition::Transition(commandList, perFrameCbuffer,
                                       D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                                       D3D12_RESOURCE_STATE_COPY_DEST);

        HLSLShared::PerFrameConstantBuffer perFrameCbufferData{
            .viewProjMatrix = frameContext.GetViewProjMatrix().Transpose(),
            .viewMatrix = frameContext.GetViewMatrix().Transpose(),
            .projMatrix = frameContext.GetProjMatrix().Transpose(),
            .invViewProjMatrix = frameContext.GetInvViewProjMatrix().Transpose(),
            .invViewMatrix = frameContext.GetInvViewMatrix().Transpose(),
            .invProjMatrix = frameContext.GetInvProjMatrix().Transpose(),
            .mainViewFrustum = Frustum(frameContext.GetViewProjMatrix()),
            .cameraWorldPos = frameContext.GetCameraPos(),
            .backbufferResolutionInvBackBufferResolution =
                Vector4(static_cast<float>(engineContext.GetBackbufferWidth()),
                        static_cast<float>(engineContext.GetBackbufferHeight()),
                        1.0f / engineContext.GetBackbufferWidth(),
                        1.0f / engineContext.GetBackbufferHeight())};

        currentUploadBuffer.Upload(&perFrameCbufferData, sizeof(perFrameCbufferData));

        commandList->CopyResource(perFrameCbuffer.Get(), currentUploadBuffer.Get());

        ResourceTransition::Transition(commandList, perFrameCbuffer, D3D12_RESOURCE_STATE_COPY_DEST,
                                       D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        Buffer& lightingCbuffer =
            resourceContainer.GetBuffer(ResourceContainer::Buf::DeferredLighting);
        UploadBuffer& lightingCbufferUploadBuffer = resourceContainer.GetFlippableUploadBuffer(
            frameIndex, ResourceContainer::FlipUploadBuf::DeferredLighting);

        // Temp lights position
        resourceTracker.Transition(lightingCbuffer, commandList, D3D12_RESOURCE_STATE_COPY_DEST);

        HLSLShared::LightingDataConstantBuffer lightingCbufferData{};
        auto& lightContainer = frameContext.GetLightContainer();
        auto& lights = lightContainer.GetLights();

        for (size_t i = 0; i < lights.size(); ++i)
        {
            Vector3 viewPos = Vector4(lights[i].worldPos, 1.0f) * frameContext.GetViewMatrix();
            lightingCbufferData.lights[i].viewPos_nearZ = Vector4(viewPos, lights[i].nearZ);
            lightingCbufferData.lights[i].color_farZ = Vector4(lights[i].color, lights[i].farZ);
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

        return true;
    }

    bool UpdateRenderPass::PrepareRendering()
    {
        return true;
    }

} // namespace Boolka