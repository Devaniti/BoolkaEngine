#include "stdafx.h"
#include "ComputeThreadContextImpl.h"

namespace Boolka
{

    ComputeThreadContextImpl::ComputeThreadContextImpl()
        : m_CommandAllocator(nullptr)
    {
    }

    ComputeThreadContextImpl::~ComputeThreadContextImpl()
    {
    }

    bool ComputeThreadContextImpl::Initialize(ComputeCommandAllocator* commandAllocator, ComputeCommandListImpl* commandList)
    {
        BLK_ASSERT(m_CommandAllocator == nullptr);
        BLK_ASSERT(m_CommandList == nullptr);

        m_CommandAllocator = commandAllocator;
        m_CommandList = commandList;

        return true;
    }

    bool ComputeThreadContextImpl::FlipCommandList(ComputeCommandListImpl* commandList)
    {
        m_CommandList = commandList;
        return m_CommandAllocator->ResetCommandList(*commandList, nullptr); // TODO replace PSO with wrapper
    }

    void ComputeThreadContextImpl::Unload()
    {
        m_CommandList = nullptr;
        m_CommandAllocator = nullptr;
    }
    
}