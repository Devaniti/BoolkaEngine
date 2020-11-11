#pragma once
#include "ComputeThreadContext.h"
#include "APIWrappers/CommandAllocator/ComputeCommandAllocator.h"
#include "APIWrappers/CommandList/ComputeCommandListImpl.h"


namespace Boolka
{

    class ComputeThreadContextImpl :
        public ComputeThreadContext
    {
    public:
        ComputeThreadContextImpl();
        ~ComputeThreadContextImpl();

        bool Initialize(ComputeCommandAllocator* commandAllocator, ComputeCommandListImpl* commandList);
        bool FlipCommandList(ComputeCommandListImpl* commandList);
        void Unload();

    private:
        ComputeCommandAllocator* m_CommandAllocator;
    };

}
