#include "stdafx.h"

#include "DebugOutputStream.h"

namespace Boolka
{

    template <typename CharType, typename Traits = std::char_traits<CharType>>
    class DebugOutputStream : public std::basic_streambuf<CharType, Traits>
    {
        using baseClass = std::basic_streambuf<CharType, Traits>;
        std::streamsize xsputn(const CharType* s, std::streamsize n) override;
        typename baseClass::int_type overflow(typename baseClass::int_type c) override;
    };

    // ASCII specialization
    template <>
    std::streamsize DebugOutputStream<char>::xsputn(const std::char_traits<char>::char_type* s,
                                                    std::streamsize n)
    {
        OutputDebugStringA(s);
        return n;
    }

    template <>
    DebugOutputStream<char>::int_type DebugOutputStream<char>::overflow(int_type c)
    {
        char data[2] = {static_cast<char>(c), '\0'};
        OutputDebugStringA(data);
        return int_type(c);
    }

    // Unicode specialization
    template <>
    std::streamsize DebugOutputStream<wchar_t>::xsputn(
        const std::char_traits<wchar_t>::char_type* s, std::streamsize n)
    {
        OutputDebugStringW(s);
        return n;
    }

    template <>
    DebugOutputStream<wchar_t>::int_type DebugOutputStream<wchar_t>::overflow(int_type c)
    {
        wchar_t data[2] = {static_cast<wchar_t>(c), '\0'};
        OutputDebugStringW(data);
        return int_type(c);
    }

    DebugOutputStream<char>* GetCharStringbuf()
    {
        static DebugOutputStream<char> charStreamBuf;
        return &charStreamBuf;
    }

    DebugOutputStream<wchar_t>* GetWCharStringbuf()
    {
        static DebugOutputStream<wchar_t> wcharStreamBuf;
        return &wcharStreamBuf;
    }

    std::ostream g_DebugOutput(GetCharStringbuf());
    std::wostream g_WDebugOutput(GetWCharStringbuf());

} // namespace Boolka
