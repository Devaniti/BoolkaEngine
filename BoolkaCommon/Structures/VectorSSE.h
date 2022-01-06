#pragma once

#ifdef BLK_USE_SSE

namespace Boolka
{

    template <>
    class [[nodiscard]] Vector<4, float>
    {
    public:
        using thisType = Vector<4, float>;

        Vector();

        ~Vector() = default;

        Vector(const Vector&) = default;
        Vector(Vector&&) = default;
        Vector& operator=(const Vector&) = default;
        Vector& operator=(Vector&&) = default;

        Vector(const __m128& data);
        Vector(std::initializer_list<float> data);
        Vector(const float* first, const float* last);

        template <size_t otherComponentCount>
        Vector(const Vector<otherComponentCount, float>& other);

        template <typename otherType>
        Vector(const Vector<4, otherType>& other);

        template <size_t firstComponentCount, typename... Args>
        Vector(const Vector<firstComponentCount, float>& first, Args... args);

        template <typename... Args>
        Vector(float first, Args... args);

        [[nodiscard]] __m128& GetInternal();
        [[nodiscard]] const __m128 GetInternal() const;

        [[nodiscard]] float x() const;
        [[nodiscard]] float y() const;
        [[nodiscard]] float z() const;
        [[nodiscard]] float w() const;

        [[nodiscard]] float& x();
        [[nodiscard]] float& y();
        [[nodiscard]] float& z();
        [[nodiscard]] float& w();

        [[nodiscard]] float r() const;
        [[nodiscard]] float g() const;
        [[nodiscard]] float b() const;
        [[nodiscard]] float a() const;

        [[nodiscard]] float& r();
        [[nodiscard]] float& g();
        [[nodiscard]] float& b();
        [[nodiscard]] float& a();

        [[nodiscard]] float& operator[](size_t i);
        [[nodiscard]] const float& operator[](size_t i) const;

        [[nodiscard]] float* GetBuffer();
        [[nodiscard]] const float* GetBuffer() const;

        [[nodiscard]] float* begin();
        [[nodiscard]] float* end();
        [[nodiscard]] const float* begin() const;
        [[nodiscard]] const float* end() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] float Dot(const thisType& other) const;
        [[nodiscard]] thisType Cross(const thisType& other) const;

        [[nodiscard]] float LengthSlow() const;
        [[nodiscard]] float LengthSqr() const;
        [[nodiscard]] thisType Normalize() const;

        [[nodiscard]] float Length3Slow() const;
        [[nodiscard]] float Length3Sqr() const;
        [[nodiscard]] thisType Normalize3() const;

        [[nodiscard]] thisType Min(const thisType& other) const;
        [[nodiscard]] thisType Max(const thisType& other) const;

        [[nodiscard]] thisType Select(const thisType& other, const thisType& mask) const;

        [[nodiscard]] thisType operator-() const;

        thisType& operator*=(float other);
        thisType& operator/=(float other);
        [[nodiscard]] thisType operator*(float other) const;
        [[nodiscard]] thisType operator/(float other) const;

        thisType& operator*=(const thisType& other);
        thisType& operator/=(const thisType& other);
        thisType& operator+=(const thisType& other);
        thisType& operator-=(const thisType& other);
        [[nodiscard]] thisType operator*(const thisType& other) const;
        [[nodiscard]] thisType operator/(const thisType& other) const;
        [[nodiscard]] thisType operator+(const thisType& other) const;
        [[nodiscard]] thisType operator-(const thisType& other) const;

        [[nodiscard]] bool operator==(const thisType& other) const;
        [[nodiscard]] bool operator!=(const thisType& other) const;

        [[nodiscard]] thisType operator>(const thisType& other) const;
        [[nodiscard]] thisType operator<(const thisType& other) const;
        [[nodiscard]] thisType operator>=(const thisType& other) const;
        [[nodiscard]] thisType operator<=(const thisType& other) const;
        [[nodiscard]] thisType EqualMask(const thisType& other) const;
        [[nodiscard]] thisType NotEqualMask(const thisType& other) const;

    protected:
        union {
            __m128 m_sse;
            float m_data[4];
        };

        template <size_t currentIndex, size_t firstComponentCount, typename... Args>
        void InitializeTemplateList(float* tmpArray,
                                    const Vector<firstComponentCount, float>& first, Args... args);
        template <size_t currentIndex, typename... Args>
        void InitializeTemplateList(float* tmpArray, float first, Args... args);
        template <size_t currentIndex>
        void InitializeTemplateList(float* tmpArray);
    };

    template <size_t otherComponentCount>
    Vector<4, float>::Vector(const Vector<otherComponentCount, float>& other)
    {

        _declspec(align(16)) float tmpArr[4] = {};
        if constexpr (otherComponentCount > 4)
        {
            std::copy(other.begin(), other.begin() + 4, tmpArr);
        }
        else
        {
            std::copy(other.begin(), other.end(), tmpArr);
        }
        m_sse = _mm_load_ps(tmpArr);
    }

    template <typename otherType>
    Vector<4, float>::Vector(const Vector<4, otherType>& other)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            m_data[i] = other[i];
        }
    }

    template <size_t firstComponentCount, typename... Args>
    Vector<4, float>::Vector(const Vector<firstComponentCount, float>& first, Args... args)
        : m_sse{}
    {
        _declspec(align(16)) float tmpArr[4] = {};
        InitializeTemplateList<0>(tmpArr, first, args...);
        m_sse = _mm_load_ps(tmpArr);
    }

    template <typename... Args>
    Vector<4, float>::Vector(float first, Args... args)
        : m_sse{}
    {
        _declspec(align(16)) float tmpArr[4] = {};
        InitializeTemplateList<0>(tmpArr, first, args...);
        m_sse = _mm_load_ps(tmpArr);
    }

    template <size_t currentIndex, size_t firstComponentCount, typename... Args>
    void Vector<4, float>::InitializeTemplateList(float* tmpArray,
                                                  const Vector<firstComponentCount, float>& first,
                                                  Args... args)
    {
        static_assert(currentIndex + firstComponentCount <= 4);
        std::copy(first.begin(), first.end(), tmpArray + currentIndex);
        InitializeTemplateList<currentIndex + firstComponentCount>(tmpArray, args...);
    }

    template <size_t currentIndex, typename... Args>
    void Vector<4, float>::InitializeTemplateList(float* tmpArray, float first, Args... args)
    {
        static_assert(currentIndex + 1 <= 4);
        tmpArray[currentIndex] = first;
        InitializeTemplateList<currentIndex + 1>(tmpArray, args...);
    }

    template <size_t currentIndex>
    void Vector<4, float>::InitializeTemplateList(float* tmpArray)
    {
        static_assert(currentIndex == 4, "Incorrect number of elements passed");
    }

} // namespace Boolka

#endif
