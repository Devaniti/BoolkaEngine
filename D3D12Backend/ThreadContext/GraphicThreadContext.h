#pragma once
#include "ComputeThreadContext.h"
#include "APIWrappers/CommandList/GraphicCommandList.h"


namespace Boolka
{

    class GraphicThreadContext :
        public ComputeThreadContext
    {
        GraphicCommandList* GetCommandList() { return static_cast<GraphicCommandList*>(m_CommandList); };

    protected:
        GraphicThreadContext() {};
        ~GraphicThreadContext() {};
    };

}
