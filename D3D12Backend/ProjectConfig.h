#pragma once

#ifdef BLK_DEBUG
#define BLK_RENDER_DEBUG
//#define BLK_USE_GPU_VALIDATION
#define USE_PIX
#endif

// Concatenates BLK_ENGINE_NAME which is a string with " Window Class"
#define BLK_WINDOW_CLASS_NAME (BLK_ENGINE_NAME L" Window Class")

#define BLK_D3D12_SEMANTIC_MAX_LENGTH 32

#define BLK_MAX_LIGHT_COUNT 4
#define BLK_SHADOWMAP_SIZE 1024

#define BLK_USE_COMMON_STATE_PROMOTION
