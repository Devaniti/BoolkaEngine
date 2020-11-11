#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class FileReader
    {
    public:
        FileReader();
        ~FileReader();

        bool OpenFile(wchar_t* filename);
        void CloseFile();

        // Returns size of file on success
        // Or 0 otherwise
        size_t StartStreaming(MemoryBlock& m_data);
        bool IsStreamed();
        bool WaitData();
        void FreeData(MemoryBlock& data);

    private:
        HANDLE m_file;
        ::OVERLAPPED m_async;
    };

}
