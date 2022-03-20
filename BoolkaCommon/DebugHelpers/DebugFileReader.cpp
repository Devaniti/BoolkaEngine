#include "stdafx.h"

#include "DebugFileReader.h"

namespace Boolka
{

    MemoryBlock DebugFileReader::ReadFile(const char* filename)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file)
            return MemoryBlock{};
        return ReadFile(file);
    }

    MemoryBlock DebugFileReader::ReadFile(const wchar_t* filename)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file)
            return MemoryBlock{};
        return ReadFile(file);
    }

    MemoryBlock DebugFileReader::ReadFile(std::ifstream& file)
    {
        BLK_ASSERT(file);
        std::streamsize size = file.tellg();
        BLK_ASSERT(size >= 0);
        file.seekg(0, std::ios::beg);
        BLK_ASSERT(file);

        MemoryBlock blob{(void*)new char[size], static_cast<size_t>(size)};
        file.read((char*)blob.m_Data, size);
        BLK_ASSERT(file);
        return blob;
    }

    void DebugFileReader::FreeMemory(MemoryBlock& data)
    {
        delete[] static_cast<char*>(data.m_Data);
        data = {};
    }

} // namespace Boolka