#pragma once
#include "BoolkaCommon/Structures/AABB.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"

// Data that always needed to be loaded for rendering
#define BLK_SCENE_REQUIRED_SCENE_DATA_FILENAME L"RequiredSceneData.blkeng"
// TODO
// Serialize built acceleration structures, invalidated on source geometry/hardware/driver version change
#define BLK_SCENE_RAYTRACING_CACHE_FILENAME L"RaytracingCache.blktmp"
#define BLK_SCENE_VERSION 1

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

        struct CPUObjectHeader
        {
            uint32_t rtIndexOffset;
            uint32_t rtIndexCount;
            uint32_t materialIndex;
        };

        static const UINT ms_SceneVersion = 0;
        struct FormatHeader
        {
            const char signature[24] = "BoolkaEngineSceneFormat";
            const UINT formatVersion = BLK_SCENE_VERSION;

            bool operator==(const FormatHeader& other) const;
        };

        struct SceneHeader
        {
            UINT vertex1Size;
            UINT vertex2Size;
            UINT vertexIndirectionSize;
            UINT indexSize;
            UINT meshletsSize;
            UINT meshletsCullSize;
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
