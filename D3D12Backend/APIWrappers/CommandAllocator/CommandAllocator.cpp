#include "stdafx.h"

#include "CommandAllocator.h"

namespace Boolka
{

    CommandAllocator::CommandAllocator()
        : m_CommandAllocator(nullptr)
    {
    }

    CommandAllocator::~CommandAllocator()
    {
        BLK_ASSERT(m_CommandAllocator == nullptr);
    }

    bool CommandAllocator::Reset()
    {
        HRESULT hr = m_CommandAllocator->Reset();
        return SUCCEEDED(hr);
    }

    ID3D12CommandAllocator* CommandAllocator::Get()
    {
        BLK_ASSERT(m_CommandAllocator != nullptr);
        return m_CommandAllocator;
    }

    ID3D12CommandAllocator* CommandAllocator::operator->()
    {
        return Get();
    }

    bool CommandAllocator::Initialize(ID3D12CommandAllocator* commandAllocator)
    {
        BLK_ASSERT(m_CommandAllocator == nullptr);

        m_CommandAllocator = commandAllocator;
        return true;
    }

    void CommandAllocator::Unload()
    {
        BLK_ASSERT(m_CommandAllocator != nullptr);

        m_CommandAllocator->Release();
        m_CommandAllocator = nullptr;
    }

} // namespace Boolka
