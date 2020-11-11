#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class DebugFileReader
    {
    public:
        static MemoryBlock ReadFile(const char* filename);
        static void FreeMemory(MemoryBlock& blob);
    };

}
