#include "stdafx.h"

#include "SceneData.h"

#include "FileReader/FileReader.h"

namespace Boolka
{

    SceneData::SceneData(FileReader& fileReader)
        : m_MemoryBlock{}
        , m_FileReader(fileReader)
    {
    }

    SceneData::~SceneData()
    {
        BLK_ASSERT(m_MemoryBlock.m_Size == 0);
        BLK_ASSERT(m_MemoryBlock.m_Data == nullptr);
    }

    SceneData::DataWrapper SceneData::GetSceneWrapper()
    {
        DataWrapper result;

        m_FileReader.WaitData(sizeof(FormatHeader) + sizeof(SceneHeader));

        const unsigned char* data = static_cast<const unsigned char*>(m_MemoryBlock.m_Data);

        const FormatHeader* formatHeader = ptr_static_cast<const FormatHeader*>(data);
        const FormatHeader correctHeader{};
        BLK_CRITICAL_ASSERT(*formatHeader == correctHeader);
        data += sizeof(FormatHeader);

        const SceneHeader* sceneHeader = ptr_static_cast<const SceneHeader*>(data);

        BLK_CRITICAL_ASSERT(sceneHeader->vertex1Size != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->vertex2Size != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->vertexIndirectionSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->indexSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->meshletsSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->objectsSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->objectCount != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->opaqueCount != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->skyBoxResolution != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->skyBoxMipCount != 0);
        BLK_CRITICAL_ASSERT(sceneHeader->textureCount != 0);

        result.header = *sceneHeader;

        data += sizeof(SceneHeader);

        result.textureHeaders = ptr_static_cast<const TextureHeader*>(data);

        data += sizeof(TextureHeader) * result.header.textureCount;

        result.binaryData = static_cast<const void*>(data);

        return result;
    }

    void SceneData::PrepareTextureHeaders()
    {
        size_t neededSize = sizeof(SceneHeader);
        const unsigned char* data = static_cast<const unsigned char*>(m_MemoryBlock.m_Data);
        const SceneHeader* sceneHeader = ptr_static_cast<const SceneHeader*>(data);

        size_t textureCount = sceneHeader->textureCount;
        neededSize += textureCount * sizeof(TextureHeader);

        bool res = m_FileReader.WaitData(neededSize);
        BLK_ASSERT_VAR(res);
    }

    void SceneData::PrepareBinaryData()
    {
        bool res = m_FileReader.WaitData();
        BLK_ASSERT_VAR(res);
    }

    const MemoryBlock& SceneData::GetMemory() const
    {
        return m_MemoryBlock;
    }

    MemoryBlock& SceneData::GetMemory()
    {
        return m_MemoryBlock;
    }

    bool SceneData::FormatHeader::operator==(const FormatHeader& other) const
    {
        return (memcmp(signature, other.signature, sizeof(signature)) == 0) && (formatVersion == other.formatVersion);
    }

} // namespace Boolka
