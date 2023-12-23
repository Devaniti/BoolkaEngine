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
#define BLK_ASSERT_VAR(variable) \
    {                            \
        if (!(variable))         \
            BLK_DEBUG_BREAK();   \
    }
#define BLK_ASSERT_VAR2(condition, variable) \
    {                                        \
        if (!(condition))                    \
            BLK_DEBUG_BREAK();               \
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
#define BLK_ASSERT_VAR(variable) BLK_UNUSED_VARIABLE(variable)
#define BLK_ASSERT_VAR2(condition, variable) BLK_UNUSED_VARIABLE(variable)
#define BLK_DEBUG_ONLY(...)

#define BLK_SET_CURRENT_THREAD_NAME(Name) BLK_NOOP()

#endif

#define BLK_SAFE_RELEASE(p) \
    {                       \
        if ((p))            \
        {                   \
            (p)->Release(); \
            (p) = nullptr;  \
        }                   \
    }
#define BLK_SAFE_DELETE(a)  \
    {                       \
        if ((a) != nullptr) \
            delete (a);     \
        (a) = nullptr;      \
    }

#define BLK_UNUSED_VARIABLE(a) (void)a

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

#define BLK_INT_DIVIDE_CEIL(numerator, denominator) ((numerator + denominator - 1) / denominator)

#define BLK_SHIFT_LEFT_WITH_CARRY(type, intValue) \
    ((intValue << 1) | ((i & (size_t(1) << (sizeof(i) * 8 - 1))) >> (sizeof(i) * 8 - 1)))
#define BLK_SHIFT_RIGHT_WITH_CARRY(type, intValue) \
    ((intValue >> 1) | ((intValue & 1) << (sizeof(intValue) * 8 - 1)))

#define BLK_INITIALIZE_ARRAY(arr, ...)               \
    {                                                \
        for (auto& elem : arr)                       \
        {                                            \
            bool res = elem.Initialize(__VA_ARGS__); \
            BLK_ASSERT_VAR(res);                     \
        }                                            \
    }
#define BLK_UNLOAD_ARRAY(arr)  \
    {                          \
        for (auto& elem : arr) \
            elem.Unload();     \
    }
#define BLK_SAFE_UNLOAD_ARRAY(arr) \
    {                              \
        for (auto& elem : arr)     \
            elem.SafeUnload();     \
    }

#define BLK_FLOAT_PI 3.141592f

#define BLK_DEG_TO_RAD(deg) (deg * BLK_FLOAT_PI / 180.0f)
#define BLK_RAD_TO_DEG(rad) (rad * 180.0f / BLK_FLOAT_PI)

#define BLK_KB(kb) (kb * 1024)
#define BLK_MB(mb) (BLK_KB(mb * 1024))
#define BLK_GB(gb) (BLK_MB(mb * 1024))

using uint = uint32_t;
using byte = uint8_t;

template <typename T>
size_t NestedVectorSize(const std::vector<std::vector<T>>& nestedVector)
{
    return std::transform_reduce(
        std::execution::seq, std::begin(nestedVector), std::end(nestedVector), size_t(0),
        std::plus<size_t>(), [](const std::vector<T>& inner) -> size_t { return inner.size(); });
}

template <typename TypeToFind, typename... List>
struct has_type : std::disjunction<std::is_same<TypeToFind, List>...>
{
};

template <typename T1, typename T2>
T1 checked_narrowing_cast(T2 value)
{
    T1 res = static_cast<T1>(value);
    BLK_ASSERT(res == value);
    return res;
}

template <typename T1, typename T2, typename intermediateType>
T1 ptr_static_cast_internal(T2 value)
{
    return static_cast<T1>(static_cast<intermediateType>(value));
}

template <typename T1, typename T2>
T1 ptr_static_cast(T2 value)
{
    using intermediateType =
        std::conditional_t<std::is_const_v<std::remove_pointer_t<T1>>, const void*, void*>;
    return ptr_static_cast_internal<T1, T2, intermediateType>(value);
}

// TODO move inline functions to cpp file

inline float asfloat(uint value)
{
    return *reinterpret_cast<float*>(&value);
}

inline uint asuint(float value)
{
    return *reinterpret_cast<uint*>(&value);
}

inline void MemcpyStrided(void* dst, size_t dstStride, const void* src, size_t srcStride,
                          size_t rows)
{
    char* currentDst = static_cast<char*>(dst);
    const char* currentSrc = static_cast<const char*>(src);

    size_t copiedStride = std::min(dstStride, srcStride);
    for (size_t i = 0; i < rows; ++i)
    {
        memcpy(currentDst, currentSrc, copiedStride);
        currentDst += dstStride;
        currentSrc += srcStride;
    }
}

inline void* OffsetPtr(void* ptr, size_t offset)
{
    return static_cast<void*>(static_cast<char*>(ptr) + offset);
}

// TODO move everything below to platform specific header
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

inline std::string UTF8encode(const std::wstring& wstr)
{
    if (wstr.empty())
        return std::string();

    int size_needed =
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

    std::string result(size_needed, 0);
    int written = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &result[0],
                                      size_needed, NULL, NULL);

    BLK_ASSERT_VAR2(written == size_needed, written);

    return result;
}

inline std::wstring UTF8decode(const std::string& str)
{
    if (str.empty())
        return std::wstring();

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

    std::wstring result(size_needed, 0);
    int written =
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size_needed);

    BLK_ASSERT_VAR2(written == size_needed, written);

    return result;
}

inline bool CombinePath(const std::wstring& source1, const std::wstring& source2,
                        std::wstring& dest)
{
    wchar_t source1lastChar = source1[source1.size() - 1];
    bool needToAddSeparator = source1lastChar != '\\' && source1lastChar != '/';

    dest.reserve(source1.size() + needToAddSeparator + source2.size() + 1);
    dest.insert(std::end(dest), std::begin(source1), std::end(source1));
    if (needToAddSeparator)
    {
        dest.push_back('\\');
    }
    dest.insert(std::end(dest), std::begin(source2), std::end(source2));
    return true;
}
