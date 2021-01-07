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
            UINT cullingSize;
            UINT indexCount;
            UINT objectCount;
            UINT textureCount;
        };

        struct DataWrapper
        {
            UINT vertexBufferSize;
            UINT indexBufferSize;
            UINT cullingBufferSize;
            UINT indexCount;
            UINT objectCount;
            UINT textureCount;
            TextureHeader* textureHeaders;
            void* binaryData;
        };

        DataWrapper GetSceneWrapper();
        void PrepareTextureHeaders();
        void PrepareBinaryData();
        MemoryBlock& GetMemory() { return m_MemoryBlock; };
        const MemoryBlock& GetMemory() const { return m_MemoryBlock; };

    private:
        MemoryBlock m_MemoryBlock;
        FileReader& m_FileReader;
    };

}
