#include "stdafx.h"
#include "CopyThreadContextImpl.h"

namespace Boolka
{

    CopyThreadContextImpl::CopyThreadContextImpl()
        : m_CommandAllocator(nullptr)
    {
    }

    CopyThreadContextImpl::~CopyThreadContextImpl()
    {
    }

    bool CopyThreadContextImpl::Initialize(CopyCommandAllocator* commandAllocator, CopyCommandListImpl* commandList)
    {
        BLK_ASSERT(m_CommandAllocator == nullptr);
        BLK_ASSERT(m_CommandList == nullptr);

        m_CommandAllocator = commandAllocator;
        m_CommandList = commandList;

        return true;
    }

    bool CopyThreadContextImpl::FlipCommandList(CopyCommandListImpl* commandList)
    {
        m_CommandList = commandList;
        return m_CommandAllocator->ResetCommandList(*commandList, nullptr); // TODO replace PSO with wrapper
    }

    void CopyThreadContextImpl::Unload()
    {
        m_CommandList = nullptr;
        m_CommandAllocator = nullptr;
    }

}
