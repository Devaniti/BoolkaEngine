#include "stdafx.h"
#include "UpdateRenderPass.h"

#include "RenderSchedule/ResourceTracker.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "APIWrappers/Resources/ResourceTransition.h"

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
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& backbuffer = resourceContainer.GetBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = resourceContainer.GetBackBufferRTV(frameIndex);
        DepthStencilView& gbufferDSV = resourceContainer.GetDSV(ResourceContainer::DSV::GbufferDepth);
        Buffer& frameConstantBuffer = resourceContainer.GetFlippableBuffer(frameIndex, ResourceContainer::FlipBuf::Frame);
        UploadBuffer& currentUploadBuffer = resourceContainer.GetFlippableUploadBuffer(frameIndex, ResourceContainer::FlipUploadBuf::Frame);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "UpdateRenderPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        ResourceTransition::Transition(frameConstantBuffer, commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);

        const Matrix4x4& viewMatrix = frameContext.GetViewMatrix();
        const Matrix4x4& projMatrix = frameContext.GetProjMatrix();
        const Matrix4x4& viewProjMatrix = frameContext.GetViewProjMatrix();
        const Matrix4x4& invViewMatrix = frameContext.GetInvViewMatrix();
        const Matrix4x4& invProjMatrix = frameContext.GetInvProjMatrix();
        const Matrix4x4& invViewProjMatrix = frameContext.GetInvViewProjMatrix();

        unsigned char* upload = static_cast<unsigned char*>(currentUploadBuffer.Map());
        memcpy(upload, viewProjMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));
        upload += sizeof(Matrix4x4);

        memcpy(upload, viewMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));
        upload += sizeof(Matrix4x4);

        memcpy(upload, projMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));
        upload += sizeof(Matrix4x4);

        memcpy(upload, invViewProjMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));
        upload += sizeof(Matrix4x4);

        memcpy(upload, invViewMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));
        upload += sizeof(Matrix4x4);

        memcpy(upload, invProjMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));
        upload += sizeof(Matrix4x4);

        currentUploadBuffer.Unmap();

        commandList->CopyResource(frameConstantBuffer.Get(), currentUploadBuffer.Get());

        ResourceTransition::Transition(frameConstantBuffer, commandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        return true;
    }

    bool UpdateRenderPass::PrepareRendering()
    {
        return true;
    }

}