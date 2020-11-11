#include "stdafx.h"
#include "DebugFileWriter.h"

namespace Boolka
{

    DebugFileWriter::DebugFileWriter() 
        : m_BytesWritten(0)
    {
    }

    bool DebugFileWriter::OpenFile(const char* filename)
    {
        m_File.open(filename, std::ios::binary | std::ios::trunc);
        BLK_ASSERT(m_File);

        return !m_File.fail();
    }

    bool DebugFileWriter::Write(MemoryBlock memoryBlock)
    {
        return Write(memoryBlock.m_Data, memoryBlock.m_Size);
    }

    bool DebugFileWriter::Write(void* data, size_t size)
    {
        m_File.write(static_cast<char*>(data), size);
        BLK_ASSERT(m_File);

        m_BytesWritten += size;

        return !m_File.fail();
    }

    bool DebugFileWriter::AddPadding(size_t size)
    {
        std::vector<unsigned char> emptyBuffer;
        emptyBuffer.resize(size);

        return Write(emptyBuffer.data(), size);
    }

    bool DebugFileWriter::Close()
    {
        static_assert(IS_POWER_OF_TWO(BLK_FILE_BLOCK_SIZE));
        size_t modulo = m_BytesWritten & (BLK_FILE_BLOCK_SIZE - 1);

        if (modulo != 0)
        {
            char dummyBuffer[BLK_FILE_BLOCK_SIZE] = {};
            size_t padding = BLK_FILE_BLOCK_SIZE - modulo;

            m_File.write(dummyBuffer, padding);
            BLK_ASSERT(m_File);
        }

        m_File.close();
        BLK_ASSERT(m_File);

        return !m_File.fail();
    }

    bool DebugFileWriter::WriteFile(const char* filename, MemoryBlock data)
    {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        BLK_ASSERT(file);

        file.write(static_cast<char*>(data.m_Data), data.m_Size);
        BLK_ASSERT(file);

        static_assert(IS_POWER_OF_TWO(BLK_FILE_BLOCK_SIZE));
        size_t modulo = data.m_Size & (BLK_FILE_BLOCK_SIZE - 1);

        if (modulo != 0)
        {
            char dummyBuffer[BLK_FILE_BLOCK_SIZE] = {};
            size_t padding = BLK_FILE_BLOCK_SIZE - modulo;

            file.write(dummyBuffer, padding);
            BLK_ASSERT(file);
        }

        file.close();

        return true;
    }

}