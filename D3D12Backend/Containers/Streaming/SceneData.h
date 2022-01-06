#pragma once
#include "BoolkaCommon/Structures/AABB.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"

// Data that always needed to be loaded for rendering
#define BLK_SCENE_REQUIRED_SCENE_DATA_FILENAME L"RequiredSceneData.blkeng"
#define BLK_SCENE_VERSION 1

namespace Boolka
{

    class FileReader;

    class [[nodiscard]] SceneData
    {
    public:
        SceneData(FileReader& fileReader);
        ~SceneData();

        struct [[nodiscard]] TextureHeader
        {
            UINT width;
            UINT height;
            UINT mipCount;
        };

        struct [[nodiscard]] CPUObjectHeader
        {
            uint32_t rtIndexOffset;
            uint32_t rtIndexCount;
            uint32_t materialIndex;
        };

        static const UINT ms_SceneVersion = 0;
        struct [[nodiscard]] FormatHeader
        {
            const char signature[24] = "BoolkaEngineSceneFormat";
            const UINT formatVersion = BLK_SCENE_VERSION;

            bool operator==(const FormatHeader& other) const;
        };

        struct [[nodiscard]] SceneHeader
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

        struct [[nodiscard]] DataWrapper
        {
            SceneHeader header;
            const TextureHeader* textureHeaders;
            const CPUObjectHeader* cpuObjectHeaders;
            const void* binaryData;
        };

        [[nodiscard]] DataWrapper GetSceneWrapper();
        void PrepareTextureHeaders();
        void PrepareCPUObjectHeaders();
        void PrepareBinaryData();
        [[nodiscard]] MemoryBlock& GetMemory();
        [[nodiscard]] const MemoryBlock& GetMemory() const;

    private:
        MemoryBlock m_MemoryBlock;
        FileReader& m_FileReader;
    };

} // namespace Boolka
