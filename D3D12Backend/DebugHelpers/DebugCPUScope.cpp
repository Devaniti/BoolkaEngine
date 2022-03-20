#include "stdafx.h"

#include "DebugCPUScope.h"

namespace Boolka
{

    DebugCPUScope::DebugCPUScope(const char* name)
    {
        StartEvent(name);
    }

    DebugCPUScope::~DebugCPUScope()
    {
        EndEvent();
    }

    void DebugCPUScope::StartEvent(const char* name)
    {
#ifdef BLK_USE_PIX_MARKERS
        PIXBeginEvent(BLK_PIX_SCOPE_COLOR, name);
#endif
    }

    void DebugCPUScope::EndEvent()
    {
#ifdef BLK_USE_PIX_MARKERS
        PIXEndEvent();
#endif
    }

} // namespace Boolka
