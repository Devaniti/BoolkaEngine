#include "stdafx.h"
#include "DebugTimer.h"

namespace Boolka
{

    DebugTimer::DebugTimer()
        : m_Frequency{}
        , m_LastTimestamp{}
    {
        BOOL res = ::QueryPerformanceFrequency(&m_Frequency);
        BLK_CRITICAL_ASSERT(res);
    }

    DebugTimer::~DebugTimer()
    {
    }

    bool DebugTimer::Start()
    {
        BOOL res = ::QueryPerformanceCounter(&m_LastTimestamp);
        BLK_CRITICAL_ASSERT(res);
        return true;
    }

    float DebugTimer::Stop()
    {
        LARGE_INTEGER currentTimestamp;
        BOOL res = ::QueryPerformanceCounter(&currentTimestamp);
        BLK_CRITICAL_ASSERT(res);

        LARGE_INTEGER timestampDifference = currentTimestamp - m_LastTimestamp;

        return timestampDifference / m_Frequency;
    }

}
