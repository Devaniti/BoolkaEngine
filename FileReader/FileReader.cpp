#include "stdafx.h"
#include "FileReader.h"
#include <iostream>

namespace Boolka
{

    FileReader::FileReader()
        : m_file(NULL)
        , m_async{}
    {
    }

    FileReader::~FileReader()
    {
        BLK_ASSERT(m_file == NULL);
    }

    bool FileReader::OpenFile(wchar_t* filename)
    {
        BLK_ASSERT(m_file == NULL);
        m_file = ::CreateFileW(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING , NULL);
        return m_file != INVALID_HANDLE_VALUE;
    }

    void FileReader::CloseFile()
    {
        BLK_ASSERT(m_file != NULL);
        ::CloseHandle(m_file);
        m_file = NULL;
    }

    size_t FileReader::StartStreaming(MemoryBlock& data)
    {
        LARGE_INTEGER size;
        BOOL res = ::GetFileSizeEx(m_file, &size);
        if (res == FALSE || size.QuadPart == 0)
        {
            return 0;
        }

        data.m_Size = size.QuadPart;
        data.m_Data = new char[data.m_Size];

        BLK_ASSERT(static_cast<DWORD>(data.m_Size) == data.m_Size);

        res = ::ReadFile(m_file, data.m_Data, static_cast<DWORD>(data.m_Size), NULL, &m_async);

        // ReadFile must return FALSE for async IO operations and set last error to ERROR_IO_PENDING 
        BLK_ASSERT(res == FALSE);
        BLK_ASSERT(GetLastError() == ERROR_IO_PENDING);

        if (GetLastError() != ERROR_IO_PENDING)
        {
            delete[] data.m_Data;
            data = {};
            return 0;
        }

        return data.m_Size;
    }

    bool FileReader::IsStreamed()
    {
        DWORD bytesRead;
        BOOL res = ::GetOverlappedResult(m_file, &m_async, &bytesRead, FALSE);
        BLK_ASSERT(res == TRUE || GetLastError() == ERROR_IO_INCOMPLETE);
        return res == TRUE;
    }

    bool FileReader::WaitData()
    {
        DWORD bytesRead;
        BOOL res = ::GetOverlappedResult(m_file, &m_async, &bytesRead, TRUE);
        BLK_ASSERT(res == TRUE);
        return res == TRUE;
    }

    bool FileReader::WaitData(size_t dataToWait)
    {
        // It seems that there's no better way to wait for part of single IO operation
        while (true)
        {
            DWORD bytesRead;
            BOOL res = ::GetOverlappedResult(m_file, &m_async, &bytesRead, FALSE);
            BLK_ASSERT(res == TRUE || GetLastError() == ERROR_IO_INCOMPLETE);
            if (bytesRead >= dataToWait || res == TRUE)
            {
                // Catch case when IO operation is done, but there was less bytes read than requested
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
        delete[] data.m_Data;
        data = {};
    }

}
