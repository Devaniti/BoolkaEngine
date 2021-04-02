#pragma once

#ifdef BLK_ENABLE_STATS

namespace Boolka
{

    struct FrameStats
    {
        float frameTime;
        float frameTimeStable;
        size_t renderedLights;
        size_t renderedLightFrustums;
    };

} // namespace Boolka

#endif
