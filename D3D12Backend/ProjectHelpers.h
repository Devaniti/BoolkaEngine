#pragma once

#ifdef BLK_RENDER_DEBUG
#define BLK_RENDER_DEBUG_ONLY(...) (__VA_ARGS__)
#else //!BLK_RENDER_DEBUG
#define BLK_RENDER_DEBUG_ONLY(...) ((void)0)
#endif
