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

    bool PresentPass::Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            resourceTracker.RegisterResource(renderContext.GetRenderEngineContext().GetSwapchainBackBuffer(i), D3D12_RESOURCE_STATE_PRESENT);
        }

        return true;
    }

    void PresentPass::Unload()
    {
    }

    bool PresentPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        UINT frameIndex = renderContext.GetRenderFrameContext().GetFrameIndex();
        resourceTracker.Transition(renderContext.GetRenderEngineContext().GetSwapchainBackBuffer(frameIndex), renderContext.GetRenderThreadContext().GetGraphicCommandList(), D3D12_RESOURCE_STATE_PRESENT);

        return true;
    }

    bool PresentPass::PrepareRendering()
    {
        return true;
    }

}