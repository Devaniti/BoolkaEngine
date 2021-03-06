#pragma once
#include "BoolkaCommon/Structures/AABB.h"
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

        struct ObjectHeader
        {
            AABB boundingBox;
            UINT startIndex;
            UINT indexCount;
        };

        struct SceneHeader
        {
            UINT vertexSize;
            UINT indexSize;
            UINT objectsSize;
            UINT indexCount;
            UINT objectCount;
            UINT opaqueCount;
            UINT textureCount;
        };

        struct DataWrapper
        {
            UINT vertexBufferSize;
            UINT indexBufferSize;
            UINT cullingBufferSize;
            UINT indexCount;
            UINT objectCount;
            UINT opaqueCount;
            UINT textureCount;
            TextureHeader* textureHeaders;
            void* binaryData;
            ObjectHeader* objectData;
        };

        DataWrapper GetSceneWrapper();
        void PrepareTextureHeaders();
        void PrepareBinaryData();
        MemoryBlock& GetMemory();
        const MemoryBlock& GetMemory() const;

    private:
        MemoryBlock m_MemoryBlock;
        FileReader& m_FileReader;
    };

} // namespace Boolka
