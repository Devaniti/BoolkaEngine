#pragma once

#ifdef BLK_ENABLE_STATS

namespace Boolka
{

    struct [[nodiscard]] FrameStats
    {
        float frameTime;
        float frameTimeStable;

        struct [[nodiscard]] GPUTimes
        {
            float Markers[static_cast<size_t>(TimestampContainer::Markers::Count)];

            GPUTimes operator*(float factor);
            GPUTimes operator+(const GPUTimes& other);
        };

        GPUTimes gpuTimes;
        GPUTimes gpuTimesStable;

        struct
        {
            uint visibleObjectCount;
            uint visibleMeshletCount;
        } visiblePerFrustum[BLK_RENDER_VIEW_COUNT];

        uint gpuDebugMarkers[BLK_DEBUG_DATA_ELEMENT_COUNT];
    };

} // namespace Boolka

#endif
