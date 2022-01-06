#include "stdafx.h"

#include "Hashing.h"

#include "Structures/MemoryBlock.h"

#define BLK_CRC32_POLYNOMIAL 0xEDB88320

namespace Boolka
{

    uint32_t Hashing::CRC32(const MemoryBlock& memory)
    {
        // Can be significantly optimized
        const char* start = reinterpret_cast<const char*>(memory.m_Data);
        const char* end = start + memory.m_Size;
        uint32_t result = 0xFFFFFFFF;

        for (const char* i = start; i < end; ++i)
        {
            char currentByte = *i;
            for (size_t j = 0; j < 8; ++j)
            {
                uint32_t bit = (currentByte ^ result) & 1;
                result >>= 1;
                result = result ^ (bit * BLK_CRC32_POLYNOMIAL);
                currentByte >>= 1;
            }
        }

        return ~result;
    }

} // namespace Boolka
