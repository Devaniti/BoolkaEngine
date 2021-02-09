#pragma once

#ifdef BLK_RENDER_DEBUG
#define BLK_RENDER_DEBUG_ONLY(...) (__VA_ARGS__)
#define BLK_PIX_SCOPE_COLOR PIX_COLOR(0, 127, 127)
#define BLK_PIX_MARKER_COLOR PIX_COLOR(127, 127, 0)
#define BLK_GPU_SCOPE(commandList, ...) DebugRenderScope BLK_UNIQUE_NAME(renderScope)(commandList, __VA_ARGS__)
#define BLK_GPU_MARKER(commandList, ...) PIXSetMarker(commandList, BLK_PIX_MARKER_COLOR, __VA_ARGS__)
#else //!BLK_RENDER_DEBUG
#define BLK_RENDER_DEBUG_ONLY(...) ((void)0)
#define BLK_GPU_SCOPE(...) ((void)0)
#define BLK_GPU_MARKER(...) ((void)0)
#endif
