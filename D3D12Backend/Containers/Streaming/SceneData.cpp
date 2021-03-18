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

        m_FileReader.WaitData(sizeof(SceneHeader));

        unsigned char* data = static_cast<unsigned char*>(m_MemoryBlock.m_Data);
        SceneHeader* sceneHeader = ptr_static_cast<SceneHeader*>(data);

        result.vertexBufferSize = sceneHeader->vertexSize;
        result.indexBufferSize = sceneHeader->indexSize;
        result.cullingBufferSize = sceneHeader->objectsSize;
        result.indexCount = sceneHeader->indexCount;
        result.textureCount = sceneHeader->textureCount;
        result.objectCount = sceneHeader->objectCount;
        result.opaqueCount = sceneHeader->opaqueCount;

        data += sizeof(SceneHeader);

        result.textureHeaders = ptr_static_cast<TextureHeader*>(data);

        data += sizeof(TextureHeader) * result.textureCount;

        result.objectData = ptr_static_cast<ObjectHeader*>(data);

        data += sceneHeader->objectsSize;

        result.binaryData = data;

        return result;
    }

    void SceneData::PrepareTextureHeaders()
    {
        size_t neededSize = sizeof(SceneHeader);
        unsigned char* data = static_cast<unsigned char*>(m_MemoryBlock.m_Data);
        SceneHeader* sceneHeader = ptr_static_cast<SceneHeader*>(data);

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

} // namespace Boolka
