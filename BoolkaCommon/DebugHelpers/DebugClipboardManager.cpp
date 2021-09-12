#include "stdafx.h"

#include "DebugClipboardManager.h"

namespace Boolka
{

    void DebugClipboardManager::SetClipboard(const wchar_t* string)
    {
        ClipboardWrapper wrapper;
        wrapper.Set(string);
    }

    void DebugClipboardManager::GetClipboard(std::wstring& string)
    {
        ClipboardWrapper wrapper;
        wrapper.Get(string);
    }

    void DebugClipboardManager::SerializeToClipboard(const wchar_t* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        wchar_t stringBuffer[512] = {};
        vswprintf(stringBuffer, ARRAYSIZE(stringBuffer), format, argList);
        SetClipboard(stringBuffer);
        va_end(argList);
    }

    void DebugClipboardManager::DeserializeFromClipboard(const wchar_t* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        std::wstring stringBuffer;
        GetClipboard(stringBuffer);
        vswscanf(stringBuffer.c_str(), format, argList);
        va_end(argList);
    }

    DebugClipboardManager::ClipboardWrapper::ClipboardWrapper()
    {
        ::OpenClipboard(NULL);
    }

    DebugClipboardManager::ClipboardWrapper::~ClipboardWrapper()
    {
        ::CloseClipboard();
    }

    void DebugClipboardManager::ClipboardWrapper::Set(const wchar_t* string)
    {
        ::EmptyClipboard();

        // Allocate and set global memory for clipboard data
        size_t neededLength = wcslen(string) + 1;
        HGLOBAL clipboardMemory = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t) * neededLength);
        wchar_t* destString = ptr_static_cast<wchar_t*>(::GlobalLock(clipboardMemory));
        ::wcscpy_s(destString, neededLength, string);
        ::GlobalUnlock(clipboardMemory);

        ::SetClipboardData(CF_UNICODETEXT, clipboardMemory);
    }

    void DebugClipboardManager::ClipboardWrapper::Get(std::wstring& string)
    {

        HGLOBAL clipboardMemory = ::GetClipboardData(CF_UNICODETEXT);
        if (!clipboardMemory)
        {
            return;
        }

        wchar_t* srcString = ptr_static_cast<wchar_t*>(::GlobalLock(clipboardMemory));
        if (!srcString)
        {
            return;
        }

        string = std::wstring(srcString);

        ::GlobalUnlock(clipboardMemory);
    }

} // namespace Boolka
