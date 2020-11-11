#include "stdafx.h"
#include "SceneData.h"


namespace Boolka
{

    SceneData::SceneData()
        : m_MemoryBlock{}
    {
    }

    SceneData::~SceneData()
    {
        BLK_ASSERT(m_MemoryBlock.m_Size == 0);
        BLK_ASSERT(m_MemoryBlock.m_Data == nullptr);
    }

    SceneData::DataWrapper SceneData::GetSceneWrapper() const
    {
        DataWrapper result;

        unsigned char* data = static_cast<unsigned char*>(m_MemoryBlock.m_Data);
        SceneHeader* sceneHeader = ptr_static_cast<SceneHeader*>(data);

        result.vertexBufferSize = sceneHeader->vertexSize;
        result.indexBufferSize = sceneHeader->indexSize;
        result.textureCount = sceneHeader->textureCount;

        data += sizeof(SceneHeader);

        result.textureHeaders = ptr_static_cast<TextureHeader*>(data);

        data += sizeof(TextureHeader) * result.textureCount;

        result.vertexData = data;

        data += result.vertexBufferSize;

        result.indexData = data;

        data += result.indexBufferSize;

        result.baseTextureData = data;

        return result;
    }

}
