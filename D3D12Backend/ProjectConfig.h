#pragma once

#if defined(BLK_CONFIGURATION_DEBUG)
#define BLK_RENDER_DEBUG
#define BLK_RENDER_DEVICE_LOST_DEBUG
#define BLK_RENDER_PROFILING
//#define BLK_USE_GPU_VALIDATION
#elif defined(BLK_CONFIGURATION_DEVELOPMENT)
#define BLK_RENDER_DEVICE_LOST_DEBUG
#define BLK_RENDER_PROFILING
#else
#define BLK_RENDER_DEVICE_LOST_DEBUG
#endif

#ifdef BLK_RENDER_PROFILING
#define BLK_USE_PIX_MARKERS
#endif

#ifdef BLK_USE_PIX_MARKERS
#define USE_PIX
#endif

// Concatenates BLK_ENGINE_NAME which is a string with " Window Class"
#define BLK_WINDOW_CLASS_NAME (BLK_ENGINE_NAME L" Window Class")

#define BLK_D3D12_SEMANTIC_MAX_LENGTH 32

#define BLK_MAX_LIGHT_COUNT 4
#define BLK_SUN_SHADOWMAP_SIZE 8192
#define BLK_LIGHT_SHADOWMAP_SIZE 1024

#define BLK_USE_COMMON_STATE_PROMOTION
