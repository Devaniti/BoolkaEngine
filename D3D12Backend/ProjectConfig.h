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

// Temporary disabled loading and saving pipeline library
// due to what seems to be Nvidia driver bug
//#define BLK_ENABLE_PIPELINE_LIBRARY_LOAD_FROM_DISK
//#define BLK_ENABLE_PIPELINE_LIBRARY_WRITE_TO_DISK

#define BLK_ENABLE_PIPELINE_LIBRARY

// Under pipeline library related flags if pipeline library itself is not enabled
#ifndef BLK_ENABLE_PIPELINE_LIBRARY
#ifdef BLK_ENABLE_PIPELINE_LIBRARY_LOAD_FROM_DISK
#undef BLK_ENABLE_PIPELINE_LIBRARY_LOAD_FROM_DISK
#endif
#ifdef BLK_ENABLE_PIPELINE_LIBRARY_WRITE_TO_DISK
#undef BLK_ENABLE_PIPELINE_LIBRARY_WRITE_TO_DISK
#endif
#endif

// Concatenates BLK_ENGINE_NAME which is a string with " Window Class"
#define BLK_WINDOW_CLASS_NAME (BLK_ENGINE_NAME L" Window Class")

#define BLK_D3D12_SEMANTIC_MAX_LENGTH 32

#define BLK_MAX_LIGHT_COUNT 4
#define BLK_SUN_SHADOWMAP_SIZE 8192
#define BLK_LIGHT_SHADOWMAP_SIZE 1024

#define BLK_USE_COMMON_STATE_PROMOTION
