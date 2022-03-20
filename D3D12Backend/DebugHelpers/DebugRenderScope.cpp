#include "stdafx.h"

#include "DebugRenderScope.h"

#ifdef BLK_USE_PIX_MARKERS

namespace Boolka
{

    DebugRenderScope::DebugRenderScope(CommandList& commandList, const char* name)
        : m_CommandList(commandList.Get())
    {
        StartEvent(name);
    }

    DebugRenderScope::~DebugRenderScope()
    {
        EndEvent();
    }

    void DebugRenderScope::StartEvent(const char* name)
    {
        PIXBeginEvent(m_CommandList, BLK_PIX_SCOPE_COLOR, name);
    }

    void DebugRenderScope::EndEvent()
    {
        PIXEndEvent(m_CommandList);
    }

} // namespace Boolka

#endif
