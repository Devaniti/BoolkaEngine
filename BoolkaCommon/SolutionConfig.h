#pragma once

// Only defines to change different stuff in the project allowed here
// Included first in ALL files

#if defined(_DEBUG) || defined(BLK_FLAG_DEBUG)
#define BLK_DEBUG
#else

#endif

#define BLK_ENABLE_STATS

#define BLK_GAME_NAME L"Test Game"
#define BLK_ENGINE_NAME L"Boolka Engine"
#define BLK_IN_FLIGHT_FRAMES 2

#define BLK_FILE_BLOCK_SIZE 4096

// 126 is not a typo, it's reccomendation by nvidia
// https://developer.nvidia.com/blog/introduction-turing-mesh-shaders/
#define BLK_MESHLET_MAX_VERTS 64
#define BLK_MESHLET_MAX_PRIMS 126

// Enables usage of SSE intrinsics
#define BLK_USE_SSE
