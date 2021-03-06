#include "stdafx.h"

#include "DebugProfileTimer.h"

#include "DebugHelpers/DebugOutputStream.h"

namespace Boolka
{

    void DebugProfileTimer::Start()
    {
        bool res = m_Timer.Start();
        BLK_ASSERT(res);
    }

    void DebugProfileTimer::Stop(const wchar_t* name)
    {
        // time in ms
        float time = m_Timer.Stop() * 1000.0f;

        g_WDebugOutput << name << L" time:" << time << L"ms" << std::endl;
    }

} // namespace Boolka
