#pragma once
#include "CopyThreadContext.h"
#include "APIWrappers/CommandList/ComputeCommandList.h"


namespace Boolka
{

    class ComputeThreadContext :
        public CopyThreadContext
    {
        ComputeCommandList* GetCommandList() { return static_cast<ComputeCommandList*>(m_CommandList); };

    protected:
        ComputeThreadContext() {};
        ~ComputeThreadContext() {};
    };

}
