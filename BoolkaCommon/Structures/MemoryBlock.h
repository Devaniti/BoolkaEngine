#pragma once

namespace Boolka
{

    struct [[nodiscard]] MemoryBlock
    {
        void* m_Data;
        size_t m_Size;
    };

} // namespace Boolka
