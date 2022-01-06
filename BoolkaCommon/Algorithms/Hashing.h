#pragma once

namespace Boolka
{

    struct MemoryBlock;

    class Hashing
    {
    public:
        [[nodiscard]] static uint32_t CRC32(const MemoryBlock& memory);
    };

} // namespace Boolka
