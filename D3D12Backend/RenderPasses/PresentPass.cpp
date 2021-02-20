#include "stdafx.h"
#include "PresentPass.h"

#include "RenderSchedule/ResourceTracker.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"

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
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "PresentPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

        UINT frameIndex = frameContext.GetFrameIndex();
        resourceTracker.Transition(resourceContainer.GetBackBuffer(frameIndex), commandList, D3D12_RESOURCE_STATE_PRESENT);

        return true;
    }

    bool PresentPass::PrepareRendering()
    {
        return true;
    }

}