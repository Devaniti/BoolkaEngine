#pragma once

#define BLK_NOOP() \
    {              \
        (void)0;   \
    }
#define BLK_DUMMY_INSTRUCTION   \
    {                           \
        volatile int dummy = 1; \
    }

#define BLK_CRITICAL_DEBUG_BREAK() \
    {                              \
        __debugbreak();            \
    }
#define BLK_CRITICAL_ASSERT(condition)  \
    {                                   \
        if (!(condition))               \
            BLK_CRITICAL_DEBUG_BREAK(); \
    }

#ifdef BLK_DEBUG

#define BLK_DEBUG_BREAK() \
    {                     \
        __debugbreak();   \
    }
#define BLK_ASSERT(condition)  \
    {                          \
        if (!(condition))      \
            BLK_DEBUG_BREAK(); \
    }
#define BLK_DEBUG_ONLY(...) (__VA_ARGS__)

// TODO move to platform dependent header
#define BLK_SET_CURRENT_THREAD_NAME(Name)                                \
    {                                                                    \
        HRESULT hr = ::SetThreadDescription(::GetCurrentThread(), Name); \
        BLK_ASSERT(SUCCEEDED(hr));                                       \
    }

#else

#define BLK_DEBUG_BREAK() BLK_NOOP()
#define BLK_ASSERT(condition) BLK_NOOP()
#define BLK_DEBUG_ONLY(...)

#define BLK_SET_CURRENT_THREAD_NAME(Name) BLK_NOOP()

#endif

#define SAFE_RELEASE(p)     \
    {                       \
        if ((p))            \
        {                   \
            (p)->Release(); \
            (p) = nullptr;  \
        }                   \
    }
#define SAFE_DELETE(a)      \
    {                       \
        if ((a) != nullptr) \
            delete (a);     \
        (a) = nullptr;      \
    }

// If you want to stringify the result of expansion of a macro argument, you have to use two levels
// of macros. https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
#define BLK_STRINGIFY(str) #str

#define BLK_INTERNAL_CONCAT(a, b) a##b
#define BLK_CONCAT(a, b) BLK_INTERNAL_CONCAT(a, b)

// Only unique in scope of single file
#define BLK_UNIQUE_NAME(baseName) BLK_CONCAT(baseName, __LINE__)

#define BLK_DECLARE_ENUM_OPERATORS(enumType)  \
    enumType operator+(enumType a, size_t b); \
    enumType& operator++(enumType& a);        \
    size_t operator-(enumType a, enumType b);

#define BLK_DEFINE_ENUM_OPERATORS(enumType)                       \
    enumType operator+(enumType a, size_t b)                      \
    {                                                             \
        return static_cast<enumType>(static_cast<size_t>(a) + b); \
    }                                                             \
    enumType& operator++(enumType& a)                             \
    {                                                             \
        return a = a + 1;                                         \
    }                                                             \
    size_t operator-(enumType a, enumType b)                      \
    {                                                             \
        return static_cast<size_t>(a) - static_cast<size_t>(b);   \
    }

#define BLK_IS_PLAIN_DATA(type)                              \
    (std::is_trivially_default_constructible<type>::value && \
     std::is_trivially_copy_constructible<type>::value &&    \
     std::is_trivially_move_constructible<type>::value &&    \
     std::is_trivially_copy_assignable<type>::value &&       \
     std::is_trivially_move_assignable<type>::value &&       \
     std::is_trivially_destructible<type>::value)

#define BLK_IS_PLAIN_DATA_ASSERT(type) \
    static_assert(BLK_IS_PLAIN_DATA(type), BLK_STRINGIFY(type) " is not plain data")

#define BLK_IS_POWER_OF_TWO(intValue) ((intValue & (intValue - 1)) == 0)
#define BLK_CEIL_TO_POWER_OF_TWO(intValue, powerOfTwo) \
    ((intValue + (powerOfTwo - 1)) & (~(powerOfTwo - 1)))
#define BLK_FLOOR_TO_POWER_OF_TWO(intValue, powerOfTwo) (intValue & (~(powerOfTwo - 1)))

#define BLK_INITIALIZE_ARRAY(arr, ...)               \
    {                                                \
        for (auto& elem : arr)                       \
        {                                            \
            bool res = elem.Initialize(__VA_ARGS__); \
            BLK_ASSERT(res);                         \
        }                                            \
    }
#define BLK_UNLOAD_ARRAY(arr)  \
    {                          \
        for (auto& elem : arr) \
            elem.Unload();     \
    }

#define BLK_FLOAT_PI 3.141592f

#define BLK_DEG_TO_RAD(deg) (deg * BLK_FLOAT_PI / 180.0f)
#define BLK_RAD_TO_DEG(rad) (rad * 180.0f / BLK_FLOAT_PI)

using uint = unsigned int;

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

inline void memcpy_strided(void* dst, size_t dstStride, const void* src, size_t srcStride,
                           size_t rows)
{
    size_t copiedStride = min(dstStride, srcStride);
    for (size_t i = 0; i < rows; ++i)
    {
        memcpy(dst, src, copiedStride);
        dst = static_cast<void*>(static_cast<unsigned char*>(dst) + dstStride);
        src = static_cast<const void*>(static_cast<const unsigned char*>(src) + srcStride);
    }
}

// TODO move to platform specific header
#define BLK_WINDOWS_DEFAULT_SCREEN_DPI 96

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
