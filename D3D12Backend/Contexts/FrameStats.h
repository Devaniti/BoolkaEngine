#pragma once

#ifdef BLK_ENABLE_STATS

namespace Boolka
{

    struct FrameStats
    {
        float frameTime;
        size_t insideFrustum;
        size_t intersectFrustum;
        size_t outsideFrustum;
    };

}

#endif
