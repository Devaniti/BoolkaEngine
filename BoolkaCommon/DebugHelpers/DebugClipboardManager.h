#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class DebugClipboardManager
    {
    public:
        static void SetClipboard(const wchar_t* string);
        static void GetClipboard(std::wstring& string);

        static void SerializeToClipboard(const wchar_t* format, ...);
        static void DeserializeFromClipboard(const wchar_t* format, ...);

    private:
        class ClipboardWrapper
        {
        public:
            ClipboardWrapper();
            ~ClipboardWrapper();

            void Set(const wchar_t* string);
            void Get(std::wstring& string);
        };
    };

} // namespace Boolka
