#include "pch.h"

#include "BoolkaCommon/Algorithms/Hashing.h"

#include "BoolkaCommon/Structures/MemoryBlock.h"

// clang-format mess up formating due to preprocessor class definition
// clang-format off

namespace Boolka
{

    TEST_CLASS(TestHashing)
    {
    public:
        TEST_METHOD(CRC32)
        {
            {
                byte number = 0x00;
                const MemoryBlock memory{&number, sizeof(number)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0xD202EF8D);
            }
            {
                byte number = 0x12;
                const MemoryBlock memory{&number, sizeof(number)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0x21BB9EC5);
            }
            {
                byte number = 0xFF;
                const MemoryBlock memory{&number, sizeof(number)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0xFF000000);
            }
            {
                uint32_t number = 0x00000000;
                const MemoryBlock memory{&number, sizeof(number)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0x2144DF1C);
            }
            {
                uint32_t number = 0x12345678;
                const MemoryBlock memory{&number, sizeof(number)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0xAF6D87D2);
            }
            {
                uint32_t number = 0xFFFFFFFF;
                const MemoryBlock memory{&number, sizeof(number)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0xFFFFFFFF);
            }
            {
                uint32_t numbers[] = {0xFFFFFFFF, 0x00000000};
                const MemoryBlock memory{numbers, sizeof(numbers)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0xFFFFFFFF);
            }
            {
                uint32_t numbers[] = {0x12345678, 0x12345678};
                const MemoryBlock memory{numbers, sizeof(numbers)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0x51705056);
            }
            {
                uint32_t numbers[] = {0x00000000, 0xFFFFFFFF};
                const MemoryBlock memory{numbers, sizeof(numbers)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0xBB99FF8A);
            }
            {
                uint32_t numbers[] = {0x00000000, 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777};
                const MemoryBlock memory{numbers, sizeof(numbers)};
                uint32_t hash = Hashing::CRC32(memory);
                Assert::IsTrue(hash == 0xC4CAC4EF);
            }
        }
    };

}
