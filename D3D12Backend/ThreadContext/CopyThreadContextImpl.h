#pragma once
#include "CopyThreadContext.h"
#include "APIWrappers/CommandAllocator/CopyCommandAllocator.h"
#include "APIWrappers/CommandList/CopyCommandListImpl.h"

namespace Boolka
{

    class CopyThreadContextImpl :
        public CopyThreadContext
    {
    public:
        CopyThreadContextImpl();
        ~CopyThreadContextImpl();

        bool Initialize(CopyCommandAllocator* commandAllocator, CopyCommandListImpl* commandList);
        bool FlipCommandList(CopyCommandListImpl* commandList);
        void Unload();

    private:
        CopyCommandAllocator* m_CommandAllocator;
    };

}
