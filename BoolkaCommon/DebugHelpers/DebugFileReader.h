#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class DebugFileReader
    {
    public:
        static MemoryBlock ReadFile(const char* filename);
        static MemoryBlock ReadFile(const wchar_t* filename);
        static MemoryBlock ReadFile(std::ifstream& file);
        static void FreeMemory(MemoryBlock& blob);
    };

} // namespace Boolka
