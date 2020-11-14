#pragma once
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    class FileReader;

    class SceneData
    {
    public:
        SceneData(FileReader& fileReader);
        ~SceneData();

        struct TextureHeader
        {
            UINT width;
            UINT height;
            UINT mipCount;
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

        DataWrapper GetSceneWrapper();
        void PrepareTextureHeaders();
        void PrepareTextures();
        MemoryBlock& GetMemory() { return m_MemoryBlock; };
        const MemoryBlock& GetMemory() const { return m_MemoryBlock; };

    private:
        MemoryBlock m_MemoryBlock;
        FileReader& m_FileReader;
    };

}
