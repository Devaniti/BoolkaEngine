#include "stdafx.h"

#include "FrameStats.h"

namespace Boolka
{

    FrameStats::GPUTimes FrameStats::GPUTimes::operator*(float factor)
    {
        GPUTimes result{};

        for (size_t i = 0; i < ARRAYSIZE(result.Markers); ++i)
        {
            result.Markers[i] = Markers[i] * factor;
        }

        return result;
    }

    FrameStats::GPUTimes FrameStats::GPUTimes::operator+(const GPUTimes& other)
    {
        GPUTimes result{};

        for (size_t i = 0; i < ARRAYSIZE(result.Markers); ++i)
        {
            result.Markers[i] = Markers[i] + other.Markers[i];
        }

        return result;
    }

} // namespace Boolka
