#include "stdafx.h"
#include "RenderFrameContext.h"


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

    void RenderFrameContext::FlipFrame(UINT frameIndex)
    {
        m_FrameIndex = frameIndex;

        LARGE_INTEGER currentTimestamp;
        BOOL res = ::QueryPerformanceCounter(&currentTimestamp);
        BLK_ASSERT(res);

        LARGE_INTEGER timestampDifference = currentTimestamp - m_LastTimestamp;

        m_LastTimestamp = currentTimestamp;

        m_DeltaTime = timestampDifference / m_Frequency;

        char buffer[256];
        snprintf(buffer, 256, "Frame %d time:%.3fms\n", frameIndex, m_DeltaTime * 1000.0f);
        OutputDebugStringA(buffer);
    }

}