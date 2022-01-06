#pragma once

#include "FileReader/FileReader.h"

#define BLK_PSO_CACHE_FILENAME L"PipelineStateLibrary.blktmp"
#define BLK_RT_CACHE_FILENAME L"RaytracingCache.blktmp"

namespace Boolka
{

    struct [[nodiscard]] RenderCacheContainer
    {
        RenderCacheContainer();

        MemoryBlock PSOCache;
        MemoryBlock RTCache;
        FileReader PSOCacheReader;
        FileReader RTCacheReader;
    };

} // namespace Boolka
