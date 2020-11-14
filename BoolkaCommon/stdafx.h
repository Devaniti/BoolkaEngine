#pragma once

// Careful, this header will be included in all projects precompiled headers

// STL
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <initializer_list>

// Windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

// Own code
#include "BoolkaCommon/SolutionConfig.h"
#include "BoolkaCommon/SolutionHelpers.h"

#include "Structures/Vector.h"
#include "Structures/Matrix.h"
