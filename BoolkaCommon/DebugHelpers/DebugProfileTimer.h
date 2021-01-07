#pragma once
#include "BoolkaCommon/DebugHelpers/DebugTimer.h"

namespace Boolka
{

    class DebugProfileTimer
    {
    public:
        void Start();
        void Stop(const wchar_t* name);

    private:
        DebugTimer m_Timer;
    };

}

