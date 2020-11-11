#pragma once
#include "ThreadContext.h"
#include "APIWrappers/CommandList/CopyCommandList.h"

namespace Boolka
{
    
    class CopyThreadContext : public ThreadContext
    {
    public:
        CopyCommandList* GetCommandList() { return static_cast<CopyCommandList*>(m_CommandList); };

    protected:
        CopyThreadContext() {};
        ~CopyThreadContext() {};
    };

}
