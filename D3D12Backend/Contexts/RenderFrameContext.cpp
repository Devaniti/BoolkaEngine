#include "stdafx.h"
#include "RenderFrameContext.h"

#include "BoolkaCommon/DebugHelpers/DebugOutputStream.h"
#include "Contexts/RenderEngineContext.h"

namespace Boolka
{

    RenderFrameContext::RenderFrameContext()
        : m_DeltaTime(0.0f)
        , m_FrameIndex(0)
        , m_LastTimestamp{}
        , m_Frequency{}
    {
    }

    RenderFrameContext::~RenderFrameContext()
    {
        BLK_ASSERT(m_DeltaTime == 0.0f);
        BLK_ASSERT(m_FrameIndex == 0);
        BLK_ASSERT(m_LastTimestamp.QuadPart == 0);
        BLK_ASSERT(m_Frequency.QuadPart == 0);
    }

    bool RenderFrameContext::Initialize(Device& device)
    {
        BOOL res = ::QueryPerformanceFrequency(&m_Frequency);
        BLK_CRITICAL_ASSERT(res);
        res = ::QueryPerformanceCounter(&m_LastTimestamp);
        BLK_CRITICAL_ASSERT(res);

        return true;
    }

    void RenderFrameContext::Unload()
    {
        m_DeltaTime = 0.0f;
        m_FrameIndex = 0;
        m_LastTimestamp = {};
        m_Frequency = {};
    }

    void RenderFrameContext::FlipFrame(RenderEngineContext& engineContext, UINT frameIndex)
    {
        m_FrameIndex = frameIndex;

        LARGE_INTEGER currentTimestamp;
        BOOL res = ::QueryPerformanceCounter(&currentTimestamp);
        BLK_ASSERT(res);

        LARGE_INTEGER timestampDifference = currentTimestamp - m_LastTimestamp;

        m_LastTimestamp = currentTimestamp;

        m_DeltaTime = timestampDifference / m_Frequency;

#ifdef BLK_ENABLE_STATS
        m_FraneStats.frameTime = m_DeltaTime;
#endif

        // Clamp max delta time to 30 fps equivalent
        m_DeltaTime = min(33.3f, m_DeltaTime);

        UINT width = engineContext.GetBackbufferWidth();
        UINT height = engineContext.GetBackbufferHeight();

        float aspectRatio = static_cast<float>(width) / height;

        engineContext.GetCamera().Update(m_DeltaTime, aspectRatio, 15.0f, 60.0f, m_ViewMatrix, m_ProjMatrix, m_CameraPos);
    }

}
