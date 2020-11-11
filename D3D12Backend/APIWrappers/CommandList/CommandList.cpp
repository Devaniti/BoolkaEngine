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
        if (m_CommandList != nullptr)
            Unload();
    }

    bool CommandList::Initialize(ID3D12GraphicsCommandList5* commandList)
    {
        BLK_ASSERT(m_CommandList == nullptr);

        m_CommandList = commandList;
        return true;
    }

    void CommandList::Unload()
    {
        BLK_ASSERT(m_CommandList != nullptr);

        m_CommandList->Release();
        m_CommandList = nullptr;
    }

}