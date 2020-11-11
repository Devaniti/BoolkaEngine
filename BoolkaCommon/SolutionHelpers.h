#pragma once

#define BLK_NOOP() { (void)0; }
#define BLK_DUMMY_INSTRUCTION { volatile int dummy = 1; }

#define BLK_CRITICAL_DEBUG_BREAK()     { __debugbreak(); }
#define BLK_CRITICAL_ASSERT(condition) { if (!(condition)) BLK_CRITICAL_DEBUG_BREAK(); }

#ifdef BLK_DEBUG

#define BLK_DEBUG_BREAK()             { __debugbreak(); }
#define BLK_ASSERT(condition)         { if (!(condition)) BLK_DEBUG_BREAK(); }
#define BLK_DEBUG_ONLY(...)           (__VA_ARGS__)

// TODO move to platform dependent header
#define SET_CURRENT_THREAD_NAME(Name) { HRESULT hr = ::SetThreadDescription(::GetCurrentThread(), Name); BLK_ASSERT(SUCCEEDED(hr)); }

#else

#define BLK_DEBUG_BREAK()             BLK_NOOP()
#define BLK_ASSERT(condition)         BLK_NOOP()
#define BLK_DEBUG_ONLY(...)

#define SET_CURRENT_THREAD_NAME(Name) BLK_NOOP()

#endif

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = nullptr; } }
#define SAFE_DELETE(a) { if( (a) != nullptr ) delete (a); (a) = nullptr; }

// If you want to stringify the result of expansion of a macro argument, you have to use two levels of macros.
// https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
#define STRINGIFY(str) #str

#define IS_PLAIN_DATA(type) (std::is_trivially_default_constructible<type>::value \
                          && std::is_trivially_copy_constructible<type>::value \
                          && std::is_trivially_move_constructible<type>::value \
                          && std::is_trivially_copy_assignable<type>::value \
                          && std::is_trivially_move_assignable<type>::value \
                          && std::is_trivially_destructible<type>::value)

#define IS_PLAIN_DATA_ASSERT(type) static_assert(IS_PLAIN_DATA(type), STRINGIFY(type) " is not plain data")

#define IS_POWER_OF_TWO(intValue) ((intValue & (intValue - 1)) == 0)
#define CEIL_TO_POWER_OF_TWO(intValue, powerOfTwo) ((intValue + (powerOfTwo - 1)) & (~(powerOfTwo - 1)))
#define FLOOR_TO_POWER_OF_TWO(intValue, powerOfTwo) (intValue & (~(powerOfTwo - 1)))

#define INITIALIZE_ARRAY(arr, ...) { for(auto& elem : arr) { bool res = elem.Initialize(__VA_ARGS__); BLK_ASSERT(res); } }
#define UNLOAD_ARRAY(arr) { for(auto& elem : arr) elem.Unload(); }

#define FLOAT_PI 3.141592f

#define DEG_TO_RAD(deg) (deg * FLOAT_PI / 180.0f)
#define RAD_TO_DEG(rad) (rad * 180.0f / FLOAT_PI)

template <typename T1, typename T2>
T1 checked_narrowing_cast(T2 value)
{
    T1 res = static_cast<T1>(value);
    BLK_ASSERT(res == value);
    return res;
}

template <typename T1, typename T2>
T1 ptr_static_cast(T2 value)
{
    return static_cast<T1>(static_cast<void*>(value));
}

// TODO move to platform specific header
inline LARGE_INTEGER operator-(LARGE_INTEGER a, LARGE_INTEGER b)
{
    LARGE_INTEGER res;
    res.QuadPart = a.QuadPart - b.QuadPart;
    return res;
}

inline float operator/(LARGE_INTEGER a, LARGE_INTEGER b)
{
    return static_cast<float>(static_cast<double>(a.QuadPart) / static_cast<double>(b.QuadPart));
}
