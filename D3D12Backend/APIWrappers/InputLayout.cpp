#include "stdafx.h"
#include "InputLayout.h"

namespace Boolka
{

    InputLayout::InputLayout()
        : m_Header({ 0 })
        , m_Entries(nullptr)
        , m_NativeEntries(nullptr)
    {
    }

    InputLayout::~InputLayout()
    {
        BLK_ASSERT(m_Header.m_NumEntries == 0);
        BLK_ASSERT(m_Entries == nullptr);
        BLK_ASSERT(m_NativeEntries == nullptr);
    }

    void InputLayout::Initialize(UINT numEntries)
    {
        BLK_ASSERT(m_Entries == nullptr);
        BLK_ASSERT(m_Header.m_NumEntries == 0);
        m_Header.m_NumEntries = numEntries;

        size_t size = numEntries * (sizeof(InputLayoutEntry) + sizeof(D3D12_INPUT_ELEMENT_DESC));
        char* memoryBlock = new char[size];

        m_Entries = reinterpret_cast<InputLayoutEntry*>(memoryBlock);
        m_NativeEntries = reinterpret_cast<D3D12_INPUT_ELEMENT_DESC*>(memoryBlock + numEntries * sizeof(InputLayoutEntry));
    }

    void InputLayout::Unload()
    {
        delete[] m_Entries;
        m_Entries = nullptr;
        m_NativeEntries = nullptr;
        m_Header.m_NumEntries = 0;
    }

    void InputLayout::SetEntry(size_t index, const D3D12_INPUT_ELEMENT_DESC& desc)
    {
        m_NativeEntries[index] = desc;
        BLK_ASSERT(strlen(desc.SemanticName) < ARRAYSIZE(m_Entries[index].SemanticName));
        strcpy_s(m_Entries[index].SemanticName, desc.SemanticName);
    }

    void InputLayout::FillInputLayoutDesc(D3D12_INPUT_LAYOUT_DESC& desc)
    {
        desc.NumElements = m_Header.m_NumEntries;
        desc.pInputElementDescs = m_NativeEntries;
    }

}
