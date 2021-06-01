#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class FileReader
    {
    public:
        FileReader();
        ~FileReader();

        bool OpenFile(const wchar_t* filename);
        void CloseFile();

        // Returns size of file on success
        // Or 0 otherwise
        size_t StartStreaming(MemoryBlock& m_data);
        bool IsStreamed();
        // Waits for first dataToWait bytes to be read
        bool WaitData(size_t dataToWait);
        // Waits for whole file to read
        bool WaitData();
        void FreeData(MemoryBlock& data);

    private:
        ::HANDLE m_file;
        ::OVERLAPPED m_async;
    };

} // namespace Boolka
