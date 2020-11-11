#include "stdafx.h"
#include "GraphicThreadContextImpl.h"

namespace Boolka
{

    GraphicThreadContextImpl::GraphicThreadContextImpl()
        : m_CommandAllocator(nullptr)
    {
    }

    GraphicThreadContextImpl::~GraphicThreadContextImpl()
    {
    }

    bool GraphicThreadContextImpl::Initialize(GraphicCommandAllocator* commandAllocator, GraphicCommandListImpl* commandList)
    {
        BLK_ASSERT(m_CommandAllocator == nullptr);
        BLK_ASSERT(m_CommandList == nullptr);

        m_CommandAllocator = commandAllocator;
        m_CommandList = commandList;

        return true;
    }

    bool GraphicThreadContextImpl::FlipCommandList(GraphicCommandListImpl* commandList)
    {
        m_CommandList = commandList;
        return m_CommandAllocator->ResetCommandList(*commandList, nullptr); // TODO replace PSO with wrapper
    }

    void GraphicThreadContextImpl::Unload()
    {
        m_CommandList = nullptr;
        m_CommandAllocator = nullptr;
    }
    
}