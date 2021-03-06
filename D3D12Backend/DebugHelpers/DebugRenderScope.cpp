#include "stdafx.h"

#include "DebugRenderScope.h"

#ifdef BLK_RENDER_DEBUG

namespace Boolka
{

    DebugRenderScope::DebugRenderScope(ID3D12GraphicsCommandList* commandList, const char* name)
        : m_commandList(commandList)
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
