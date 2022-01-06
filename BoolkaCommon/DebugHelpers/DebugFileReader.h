#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class DebugFileReader
    {
    public:
        [[nodiscard]] static MemoryBlock ReadFile(const char* filename);
        [[nodiscard]] static MemoryBlock ReadFile(const wchar_t* filename);
        [[nodiscard]] static MemoryBlock ReadFile(std::ifstream& file);
        static void FreeMemory(MemoryBlock& blob);
    };

} // namespace Boolka
