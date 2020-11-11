#pragma once
#include "GraphicThreadContext.h"
#include "APIWrappers/CommandAllocator/GraphicCommandAllocator.h"
#include "APIWrappers/CommandList/GraphicCommandListImpl.h"


namespace Boolka
{

    class GraphicThreadContextImpl :
        public GraphicThreadContext
    {
    public:
        GraphicThreadContextImpl();
        ~GraphicThreadContextImpl();

        bool Initialize(GraphicCommandAllocator* commandAllocator, GraphicCommandListImpl* commandList);
        bool FlipCommandList(GraphicCommandListImpl* commandList);
        void Unload();

    private:
        GraphicCommandAllocator* m_CommandAllocator;
    };

}
