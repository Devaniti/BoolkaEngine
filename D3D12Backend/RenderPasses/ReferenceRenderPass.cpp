#include "stdafx.h"

#include "ReferenceRenderPass.h"

#include "APIWrappers/CommandList/GraphicCommandListImpl.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool ReferenceRenderPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        // Uncomment and replace class name when creating new RenderPass
        // Also add new class to TimestampContainer::Markers
        //BLK_RENDER_PASS_START(ReferenceRenderPass);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        BLK_UNUSED_VARIABLE(frameConstantBuffer); // Remove when creating new RenderPass

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();
        BLK_UNUSED_VARIABLE(commandList); // Remove when creating new RenderPass

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