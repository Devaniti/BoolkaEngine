#pragma once

// Only defines to change different stuff in the project allowed here
// Included first in ALL files

#ifdef _DEBUG
#define BLK_DEBUG
#else

#endif

#define BLK_ENABLE_STATS

#define BLK_GAME_NAME L"Test Game"
#define BLK_ENGINE_NAME L"Boolka Engine"
#define BLK_IN_FLIGHT_FRAMES 2

#define BLK_FILE_BLOCK_SIZE 4096

// Enables usage of SSE intrinsics
#define BLK_USE_SSE
