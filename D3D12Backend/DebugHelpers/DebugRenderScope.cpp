#include "stdafx.h"

#include "DebugRenderScope.h"

#ifdef BLK_USE_PIX_MARKERS

namespace Boolka
{

    DebugRenderScope::DebugRenderScope(CommandList& commandList, const char* name)
        : m_commandList(commandList.Get())
    {
        StartEvent(name);
    }

    DebugRenderScope::~DebugRenderScope()
    {
        EndEvent();
    }

    void DebugRenderScope::StartEvent(const char* name)
    {
        PIXBeginEvent(m_commandList, BLK_PIX_SCOPE_COLOR, name);
    }

    void DebugRenderScope::EndEvent()
    {
        PIXEndEvent(m_commandList);
    }

} // namespace Boolka

#endif
