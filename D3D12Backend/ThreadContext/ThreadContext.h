#pragma once
#include "APIWrappers/CommandList/CommandList.h"

namespace Boolka
{
    
    class ThreadContext
    {
    public:
        CommandList* GetCommandList() { return m_CommandList; };

    protected:
        ThreadContext();
        ~ThreadContext();

        ThreadContext(const ThreadContext&) = delete;
        ThreadContext(ThreadContext&&) = delete;
        ThreadContext& operator=(const ThreadContext&) = delete;
        ThreadContext& operator=(ThreadContext&&) = delete;

        CommandList* m_CommandList;
    };

}
