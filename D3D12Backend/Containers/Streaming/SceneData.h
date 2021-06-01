#pragma once
#include "BoolkaCommon/Structures/AABB.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"

// Data that always needed to be loaded for rendering
#define BLK_SCENE_REQUIRED_SCENE_DATA_FILENAME L"RequiredSceneData.blkeng"
// TODO
// Built acceleration strucres, invalidated on source geometry/hardware/driver version change
#define BLK_SCENE_RAYTRACING_CACHE_FILENAME L"RaytracingCache.blktmp"

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
        };

        static_assert(sizeof(MeshletData) % 16 == 0,
                      "This struct is used in structured buffer, so for performance reasons its "
                      "size should be multiple of float4");

        struct RTVertexAdditionalData
        {
            Vector3 normal;
            Vector2 UV;
            Vector3 padding;
        };

        static_assert(sizeof(RTVertexAdditionalData) % 16 == 0,
                      "This struct is used in structured buffer, so for performance reasons its "
                      "size should be multiple of float4");

        struct CPUObjectHeader
        {
            uint32_t rtIndexOffset;
            uint32_t rtIndexCount;
            uint32_t materialIndex;
        };

        static const UINT ms_SceneVersion = 0;
        struct FormatHeader
        {
            char signature[24] = "BoolkaEngineSceneFormat";
            UINT formatVersion = ms_SceneVersion;

            bool operator==(const FormatHeader& other) const;
        };

        struct SceneHeader
        {
            UINT vertex1Size;
            UINT vertex2Size;
            UINT vertexIndirectionSize;
            UINT indexSize;
            UINT meshletsSize;
            UINT objectsSize;
            UINT materialsSize;
            UINT rtIndiciesSize;
            UINT rtObjectIndexOffsetSize;
            UINT objectCount;
            UINT opaqueCount;
            UINT skyBoxResolution;
            UINT skyBoxMipCount;
            UINT textureCount;
        };

        struct DataWrapper
        {
            SceneHeader header;
            const TextureHeader* textureHeaders;
            const CPUObjectHeader* cpuObjectHeaders;
            const void* binaryData;
        };

        DataWrapper GetSceneWrapper();
        void PrepareTextureHeaders();
        void PrepareCPUObjectHeaders();
        void PrepareBinaryData();
        MemoryBlock& GetMemory();
        const MemoryBlock& GetMemory() const;

    private:
        MemoryBlock m_MemoryBlock;
        FileReader& m_FileReader;
    };

} // namespace Boolka
