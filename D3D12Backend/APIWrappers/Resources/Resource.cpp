#include "stdafx.h"

#include "Resource.h"

namespace Boolka
{

    ID3D12Resource* Resource::Get()
    {
        BLK_ASSERT(m_Resource != nullptr);
        return m_Resource;
    }

    ID3D12Resource* Resource::operator->()
    {
        return Get();
    }

#ifdef BLK_RENDER_DEBUG

    void Resource::SetDebugName(const wchar_t* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        wchar_t name[100];
        // Parses printf format and parameters from va_list and create wide char
        // string
        _vsnwprintf_s(name, ARRAYSIZE(name), format, argList);
        va_end(argList);
        m_Resource->SetName(name);
    }

#endif

    Resource::Resource()
        : m_Resource(nullptr)
    {
    }

    Resource::~Resource()
    {
        BLK_ASSERT(m_Resource == nullptr);
    }

} // namespace Boolka
