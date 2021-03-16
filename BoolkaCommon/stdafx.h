#pragma once

// Careful, this header will be included in all projects precompiled headers

// Solution configuration
// only defines here
#include "BoolkaCommon/SolutionConfig.h"

// STL
#include <algorithm>
#include <array>
#include <atomic>
#include <execution>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

// Intrinsics
#ifdef BLK_USE_SSE
#include <xmmintrin.h>
#include <intrin.h>
#endif

// Own common code
#include "BoolkaCommon/SolutionHelpers.h"
#include "Structures/Matrix.h"
#include "Structures/Vector.h"

#ifdef BLK_DEBUG
#include "DebugHelpers/DebugOutputStream.h"
#endif
