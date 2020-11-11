#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class DebugFileWriter
    {
    public:
        DebugFileWriter();
        ~DebugFileWriter() = default;

        bool OpenFile(const char* filename);
        bool Write(MemoryBlock memoryBlock);
        bool Write(void* data, size_t size);
        bool AddPadding(size_t size);
        bool Close();

        // Compact way of writing file from single MemoryBlock
        static bool WriteFile(const char* filename, MemoryBlock data);
    private:
        std::ofstream m_File;
        size_t m_BytesWritten;
    };

}
