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

        struct VertexData1
        {
            float position[3];
            float textureCoordX;
        };

        static_assert(sizeof(VertexData1) % 16 == 0,
                      "This struct is used in structured buffer, so for performance reasons its "
                      "size should be multiple of float4");

        struct VertexData2
        {
            float normal[3];
            float textureCoordY;
        };

        static_assert(sizeof(VertexData2) % 16 == 0,
                      "This struct is used in structured buffer, so for performance reasons its "
                      "size should be multiple of float4");

        struct ObjectHeader
        {
            AABB boundingBox;
            uint32_t materialIndex;
            uint32_t meshletOffset;
            uint32_t meshletCount;
            uint32_t unused;
        };

        static_assert(sizeof(ObjectHeader) % 16 == 0,
                      "This struct is used in structured buffer, so for performance reasons its "
                      "size should be multiple of float4");

        struct MeshletData
        {
            uint32_t VertCount;
            uint32_t VertOffset;
            uint32_t PrimCount;
            uint32_t PrimOffset;

            Vector4 CullingData;
        };

        static_assert(sizeof(MeshletData) % 16 == 0,
                      "This struct is used in structured buffer, so for performance reasons its "
                      "size should be multiple of float4");

        struct SceneHeader
        {
            UINT vertex1Size;
            UINT vertex2Size;
            UINT vertexIndirectionSize;
            UINT indexSize;
            UINT meshletsSize;
            UINT objectsSize;
            UINT objectCount;
            UINT opaqueCount;
            UINT textureCount;
        };

        struct DataWrapper
        {
            SceneHeader header;
            const TextureHeader* textureHeaders;
            const void* binaryData;
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
