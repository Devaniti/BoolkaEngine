#include "stdafx.h"
#include "DebugFileWriter.h"

namespace Boolka
{

    DebugFileWriter::DebugFileWriter() 
        : m_BytesWritten(0)
    {
    }

    DebugFileWriter::~DebugFileWriter()
    {
        BLK_ASSERT(!m_File.is_open());
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

    bool DebugFileWriter::Write(const void* data, size_t size)
    {
        m_File.write(static_cast<const char*>(data), size);
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

    bool DebugFileWriter::Close(size_t alignment /*= 0*/)
    {
        BLK_ASSERT(IS_POWER_OF_TWO(alignment));

        if (alignment != 0)
        {
            size_t modulo = m_BytesWritten & (alignment - 1);
            if (modulo != 0)
            {
                bool res = AddPadding(alignment - modulo);
                if (!res)
                {
                    return false;
                }
            }
        }

        m_File.close();
        BLK_ASSERT(m_File);

        return !m_File.fail();
    }

    bool DebugFileWriter::WriteFile(const char* filename, MemoryBlock data, size_t alignment /*= 0*/)
    {
        DebugFileWriter fileWriter;
        bool res = fileWriter.OpenFile(filename);
        if (!res)
        {
            return false;
        }

        res = fileWriter.Write(data);
        if (!res)
        {
            return false;
        }

        res = fileWriter.Close(alignment);
        if (!res)
        {
            return false;
        }

        return true;
    }

}