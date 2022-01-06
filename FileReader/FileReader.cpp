#include "stdafx.h"

#include "FileReader.h"

#include <iostream>

namespace Boolka
{

    FileReader::FileReader()
        : m_File(NULL)
        , m_Async{}
    {
    }

    FileReader::~FileReader()
    {
        BLK_ASSERT(m_File == NULL);
    }

    bool FileReader::OpenFile(const wchar_t* filename, bool enableOSBuffering /*= false*/)
    {
        BLK_ASSERT(m_File == NULL);
        m_File = ::CreateFileW(
            filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED | (enableOSBuffering ? 0 : FILE_FLAG_NO_BUFFERING), NULL);
        if (m_File == INVALID_HANDLE_VALUE)
        {
            m_File = NULL;
            return false;
        }
        return true;
    }

    void FileReader::CloseFile()
    {
        BLK_ASSERT(m_File != NULL);
        ::CloseHandle(m_File);
        m_File = NULL;
    }

    bool FileReader::IsOpen()
    {
        return m_File != NULL;
    }

    bool FileReader::StartStreaming(MemoryBlock& data)
    {
        LARGE_INTEGER size;
        BOOL res = ::GetFileSizeEx(m_File, &size);
        if (res == FALSE || size.QuadPart == 0)
        {
            return false;
        }

        data.m_Size = size.QuadPart;
        data.m_Data = VirtualAlloc(NULL, data.m_Size, MEM_COMMIT, PAGE_READWRITE);

        BLK_ASSERT(static_cast<DWORD>(data.m_Size) == data.m_Size);

        res = ::ReadFile(m_File, data.m_Data, static_cast<DWORD>(data.m_Size), NULL, &m_Async);

        // ReadFile must return FALSE for async IO operations and set last error to ERROR_IO_PENDING
        BLK_ASSERT_VAR2(res == FALSE, res);
        BLK_ASSERT(GetLastError() == ERROR_IO_PENDING);

        if (GetLastError() != ERROR_IO_PENDING)
        {
            FreeData(data);
            return false;
        }

        return true;
    }

    bool FileReader::IsStreamed()
    {
        DWORD bytesRead;
        BOOL res = ::GetOverlappedResult(m_File, &m_Async, &bytesRead, FALSE);
        BLK_ASSERT(res == TRUE || GetLastError() == ERROR_IO_INCOMPLETE);
        return res == TRUE;
    }

    bool FileReader::WaitData()
    {
        DWORD bytesRead;
        BOOL res = ::GetOverlappedResult(m_File, &m_Async, &bytesRead, TRUE);
        BLK_ASSERT(res == TRUE);
        return res == TRUE;
    }

    bool FileReader::WaitData(size_t dataToWait)
    {
        // It seems that there's no better way to wait for part of single IO operation
        while (true)
        {
            DWORD bytesRead;
            BOOL res = ::GetOverlappedResult(m_File, &m_Async, &bytesRead, FALSE);
            BLK_ASSERT(res == TRUE || GetLastError() == ERROR_IO_INCOMPLETE);
            if (bytesRead >= dataToWait || res == TRUE)
            {
                // Catch case when IO operation is done, but there was less bytes read than
                // requested
                BLK_ASSERT(bytesRead >= dataToWait);
                break;
            }
            ::Sleep(1);
        }
        return true;
    }

    void FileReader::FreeData(MemoryBlock& data)
    {
        BLK_ASSERT(data.m_Size != 0);
        BLK_ASSERT(data.m_Data != nullptr);
        VirtualFree(data.m_Data, 0, MEM_RELEASE);
        data = {};
    }

} // namespace Boolka
