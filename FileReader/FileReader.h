#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class [[nodiscard]] FileReader
    {
    public:
        FileReader();
        ~FileReader();

        bool OpenFile(const wchar_t* filename, bool enableOSBuffering = false);
        void CloseFile();

        [[nodiscard]] bool IsOpen();

        bool StartStreaming(MemoryBlock& data);
        [[nodiscard]] bool IsStreamed();
        // Waits for first dataToWait bytes to be read
        bool WaitData(size_t dataToWait);
        // Waits for whole file to read
        bool WaitData();
        void FreeData(MemoryBlock& data);

    private:
        ::HANDLE m_File;
        ::OVERLAPPED m_Async;
    };

} // namespace Boolka
