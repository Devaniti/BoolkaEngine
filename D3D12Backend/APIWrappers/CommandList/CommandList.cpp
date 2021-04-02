#include "stdafx.h"

#include "CommandList.h"

namespace Boolka
{

    CommandList::CommandList()
        : m_CommandList(nullptr)
    {
    }

    CommandList::~CommandList()
    {
        BLK_ASSERT(m_CommandList == nullptr);
    }

    bool CommandList::Initialize(ID3D12GraphicsCommandList6* commandList)
    {
        BLK_ASSERT(m_CommandList == nullptr);

        m_CommandList = commandList;
        return true;
    }

    ID3D12GraphicsCommandList6* CommandList::Get()
    {
        BLK_ASSERT(m_CommandList != nullptr);
        return m_CommandList;
    }

    ID3D12GraphicsCommandList6* CommandList::operator->()
    {
        return Get();
    }

    void CommandList::Unload()
    {
        BLK_ASSERT(m_CommandList != nullptr);

        m_CommandList->Release();
        m_CommandList = nullptr;
    }

} // namespace Boolka