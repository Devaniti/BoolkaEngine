#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class SceneData
    {
    public:
        SceneData();
        ~SceneData();

        struct TextureHeader
        {
            UINT width;
            UINT height;
        };

        struct SceneHeader
        {
            UINT vertexSize;
            UINT indexSize;
            int textureCount;
        };

        struct DataWrapper
        {
            UINT vertexBufferSize;
            UINT indexBufferSize;
            int textureCount;
            void* vertexData;
            void* indexData;
            TextureHeader* textureHeaders;
            void* baseTextureData;
        };

        DataWrapper GetSceneWrapper() const;
        MemoryBlock& GetMemory() { return m_MemoryBlock; };
        const MemoryBlock& GetMemory() const { return m_MemoryBlock; };

    private:
        MemoryBlock m_MemoryBlock;
    };

}
