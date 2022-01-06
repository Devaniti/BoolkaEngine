#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class [[nodiscard]] DebugFileWriter
    {
    public:
        DebugFileWriter();
        ~DebugFileWriter();

        bool OpenFile(const char* filename);
        bool OpenFile(const wchar_t* filename);
        bool Write(MemoryBlock memoryBlock);
        bool Write(const void* data, size_t size);
        bool AddPadding(size_t size);
        bool Close(size_t alignment = 0);

        // Compact way of writing file from single MemoryBlock
        static bool WriteFile(const char* filename, MemoryBlock data, size_t alignment = 0);
        static bool WriteFile(const wchar_t* filename, MemoryBlock data, size_t alignment = 0);

    private:
        static bool WriteFile(DebugFileWriter& fileWriter, MemoryBlock data, size_t alignment);

        std::ofstream m_File;
        size_t m_BytesWritten;
    };

} // namespace Boolka
