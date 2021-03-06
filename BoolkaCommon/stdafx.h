#pragma once

// Careful, this header will be included in all projects precompiled headers

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

// Own code
#include "BoolkaCommon/SolutionConfig.h"
#include "BoolkaCommon/SolutionHelpers.h"
#include "Structures/Matrix.h"
#include "Structures/Vector.h"
