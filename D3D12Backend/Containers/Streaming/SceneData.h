#pragma once

// Data that always needed to be loaded for rendering
#define BLK_SCENE_HEADER_FILENAME L"SceneHeader.blkeng"
#define BLK_SCENE_DATA_FILENAME L"SceneData.blkeng"
#define BLK_SCENE_VERSION 2

#define BLK_CACHE_RT_HEADER_FILENAME L"RaytracingCacheHeader.blktmp"
#define BLK_CACHE_RT_FILENAME L"RaytracingCache.blktmp"

#define BLK_SCENE_MAX_ALLOWED_BUFFER_SIZE BLK_MB(256)

namespace Boolka
{
    namespace SceneData
    {

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

        struct [[nodiscard]] FormatHeader
        {
            const char signature[24] = "BoolkaEngineSceneFormat";
            const UINT formatVersion = BLK_SCENE_VERSION;

            bool IsValid() const;
        };

        struct [[nodiscard]] SceneHeader
        {
            // Random number generated when building scene
            // Can be used to determine whether certain caches need to be cleared
            UINT sceneIdentifier;
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

    } // namespace SceneData
} // namespace Boolka
