#include "stdafx.h"

#include "PresentPass.h"

#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool PresentPass::Initialize(Device& device, RenderContext& renderContext)
    {
        return true;
    }

    void PresentPass::Unload()
    {
    }

    bool PresentPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(PresentPass);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        UINT frameIndex = frameContext.GetFrameIndex();
        resourceTracker.Transition(resourceContainer.GetBackBuffer(frameIndex), commandList,
                                   D3D12_RESOURCE_STATE_PRESENT);

        engineContext.GetTimestampContainer().Mark(commandList,
                                                   TimestampContainer::Markers::EndFrame);
        engineContext.GetTimestampContainer().FinishFrame(commandList, renderContext, frameIndex);

        return true;
    }

    bool PresentPass::PrepareRendering()
    {
        return true;
    }

} // namespace Boolka