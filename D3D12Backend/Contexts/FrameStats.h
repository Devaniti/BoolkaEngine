#pragma once

#ifdef BLK_ENABLE_STATS

namespace Boolka
{

    struct FrameStats
    {
        float frameTime;
        float frameTimeStable;

        struct GPUTimes
        {
            float Markers[static_cast<size_t>(TimestampContainer::Markers::Count)];

            GPUTimes operator*(float factor);
            GPUTimes operator+(const GPUTimes& other);
        };

        GPUTimes gpuTimes;
        GPUTimes gpuTimesStable;
    };

} // namespace Boolka

#endif
