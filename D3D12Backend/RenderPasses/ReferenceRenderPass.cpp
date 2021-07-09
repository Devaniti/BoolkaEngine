#include "stdafx.h"

#include "ReferenceRenderPass.h"

namespace Boolka
{

    bool ReferenceRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        BLK_UNUSED_VARIABLE(frameConstantBuffer); // Remove when creating new RenderPass

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();
        BLK_UNUSED_VARIABLE(commandList); // Remove when creating new RenderPass

        BLK_GPU_SCOPE(commandList.Get(), "ReferenceRenderPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        return true;
    }

    bool ReferenceRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool ReferenceRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();
        auto& defaultRootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);
        BLK_UNUSED_VARIABLE(defaultRootSig); // Remove when creating new RenderPass

        return true;
    }

    void ReferenceRenderPass::Unload()
    {
    }

} // namespace Boolka