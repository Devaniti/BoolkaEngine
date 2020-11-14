#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class DebugFileWriter
    {
    public:
        DebugFileWriter();
        ~DebugFileWriter();;

        bool OpenFile(const char* filename);
        bool Write(MemoryBlock memoryBlock);
        bool Write(const void* data, size_t size);
        bool AddPadding(size_t size);
        bool Close(size_t alignment = 0);

        // Compact way of writing file from single MemoryBlock
        static bool WriteFile(const char* filename, MemoryBlock data, size_t alignment = 0);
    private:
        std::ofstream m_File;
        size_t m_BytesWritten;
    };

}
