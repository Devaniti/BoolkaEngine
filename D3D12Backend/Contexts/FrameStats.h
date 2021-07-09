#pragma once

#ifdef BLK_ENABLE_STATS

namespace Boolka
{

    struct FrameStats
    {
        float frameTime;
        float frameTimeStable;

        struct
        {
            uint visibleObjectCount;
            uint visibleMeshletCount;
        } visiblePerFrustum[BLK_RENDER_VIEW_COUNT];

        uint gpuDebugMarkers[256];
    };

} // namespace Boolka

#endif
