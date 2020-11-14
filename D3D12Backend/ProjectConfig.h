#pragma once

#ifdef BLK_DEBUG
#define BLK_RENDER_DEBUG
#endif

// Concatenates BLK_ENGINE_NAME which is a string with " Window Class"
#define BLK_WINDOW_CLASS_NAME (BLK_ENGINE_NAME L" Window Class")

#define BLK_D3D12_SEMANTIC_MAX_LENGTH 32

#define BLK_USE_COMMON_STATE_PROMOTION
