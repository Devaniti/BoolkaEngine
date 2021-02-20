#include "stdafx.h"
#include "Resource.h"

namespace Boolka
{

    Resource::Resource()
        : m_Resource(nullptr)
    {
    }

    Resource::~Resource()
    {
        BLK_ASSERT(m_Resource == nullptr);
    }

#ifdef BLK_RENDER_DEBUG
    void Resource::SetDebugName(const wchar_t* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        wchar_t name[100];
        // Parses printf format and parameters from va_list and create wide char string
        _vsnwprintf_s(name, ARRAYSIZE(name), format, argList);
        va_end(argList);
        m_Resource->SetName(name);
    }
#endif

}
