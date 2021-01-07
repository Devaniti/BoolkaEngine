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

    bool UpdateRenderPass::Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        return true;
    }

    void UpdateRenderPass::Unload()
    {
    }

    bool UpdateRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        UINT frameIndex = frameContext.GetFrameIndex();
        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        Buffer& currentConstantBuffer = engineContext.GetConstantBuffer(frameIndex);
        UploadBuffer& currentUploadBuffer = engineContext.GetConstantUploadBuffer(frameIndex);

        ResourceTransition::Transition(currentConstantBuffer, commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);

        Matrix4x4 viewProjMatrix = frameContext.GetViewMatrix() * frameContext.GetProjMatrix();

        unsigned char* upload = static_cast<unsigned char*>(currentUploadBuffer.Map());
        memcpy(upload, viewProjMatrix.Transpose().GetBuffer(), sizeof(Matrix4x4));
        upload += sizeof(Matrix4x4);

        memcpy(upload, frameContext.GetViewMatrix().Transpose().GetBuffer(), sizeof(Matrix4x4));

        currentUploadBuffer.Unmap();

        commandList->CopyResource(currentConstantBuffer.Get(), currentUploadBuffer.Get());

        ResourceTransition::Transition(currentConstantBuffer, commandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        return true;
    }

    bool UpdateRenderPass::PrepareRendering()
    {
        return true;
    }

}