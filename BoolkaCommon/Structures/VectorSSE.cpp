#include "stdafx.h"

#ifdef BLK_USE_SSE

#include "VectorSSE.h"

namespace Boolka
{

    Vector<4, float>::Vector()
        : m_data{}
    {
    }

    Vector<4, float>::Vector(const __m128& data)
        : m_sse(data)
    {
    }

    Vector<4, float>::Vector(std::initializer_list<float> data)
        : m_data{}
    {
        BLK_ASSERT(data.size() <= 4);
        std::copy(data.begin(), data.end(), m_data);
    }

    Vector<4, float>::Vector(const float* first, const float* last)
        : m_data{}
    {
        BLK_ASSERT(last - first <= 4);
        std::copy(first, last, m_data);
    }

    __m128& Vector<4, float>::GetInternal()
    {
        return m_sse;
    }

    const __m128 Vector<4, float>::GetInternal() const
    {
        return m_sse;
    }

    float Vector<4, float>::x() const
    {
        return m_data[0];
    };

    float Vector<4, float>::y() const
    {
        return m_data[1];
    };

    float Vector<4, float>::z() const
    {
        return m_data[2];
    };

    float Vector<4, float>::w() const
    {
        return m_data[3];
    };

    float& Vector<4, float>::x()
    {
        return m_data[0];
    };

    float& Vector<4, float>::y()
    {
        return m_data[1];
    };

    float& Vector<4, float>::z()
    {
        return m_data[2];
    };

    float& Vector<4, float>::w()
    {
        return m_data[3];
    };

    float Vector<4, float>::r() const
    {
        return m_data[0];
    };

    float Vector<4, float>::g() const
    {
        return m_data[1];
    };

    float Vector<4, float>::b() const
    {
        return m_data[2];
    };

    float Vector<4, float>::a() const
    {
        return m_data[3];
    };

    float& Vector<4, float>::r()
    {
        return m_data[0];
    };

    float& Vector<4, float>::g()
    {
        return m_data[1];
    };

    float& Vector<4, float>::b()
    {
        return m_data[2];
    };

    float& Vector<4, float>::a()
    {
        return m_data[3];
    };

    float& Vector<4, float>::operator[](size_t i)
    {
        BLK_ASSERT(i < 4);
        return m_data[i];
    };

    const float& Vector<4, float>::operator[](size_t i) const
    {
        BLK_ASSERT(i < 4);
        return m_data[i];
    };

    float* Vector<4, float>::GetBuffer()
    {
        return m_data;
    }

    const float* Vector<4, float>::GetBuffer() const
    {
        return m_data;
    }

    float* Vector<4, float>::begin()
    {
        return m_data;
    }

    float* Vector<4, float>::end()
    {
        return m_data + 4;
    }

    const float* Vector<4, float>::begin() const
    {
        return m_data;
    }

    const float* Vector<4, float>::end() const
    {
        return m_data + 4;
    }

    size_t Vector<4, float>::size() const
    {
        return 4;
    }

    float Vector<4, float>::Dot(const thisType& other) const
    {
        __m128 result = _mm_dp_ps(m_sse, other.m_sse, 0xff);
        _declspec(align(16)) float tmpArr[4];
        _mm_store_ps(tmpArr, result);
        return tmpArr[0];
    }

    Vector<4, float> Vector<4, float>::Cross(const thisType& other) const
    {
        return _mm_sub_ps(
            _mm_mul_ps(_mm_shuffle_ps(m_sse, m_sse, _MM_SHUFFLE(3, 0, 2, 1)),
                       _mm_shuffle_ps(other.m_sse, other.m_sse, _MM_SHUFFLE(3, 1, 0, 2))),
            _mm_mul_ps(_mm_shuffle_ps(m_sse, m_sse, _MM_SHUFFLE(3, 1, 0, 2)),
                       _mm_shuffle_ps(other.m_sse, other.m_sse, _MM_SHUFFLE(3, 0, 2, 1))));
    }

    float Vector<4, float>::LengthSlow() const
    {
        return ::sqrt(LengthSqr());
    }

    float Vector<4, float>::LengthSqr() const
    {
        __m128 tmpVec = _mm_dp_ps(m_sse, m_sse, 0xff);
        _declspec(align(16)) float tmpArr[4];
        _mm_store_ps(tmpArr, tmpVec);
        return tmpArr[0];
    }

    Vector<4, float> Vector<4, float>::Normalize() const
    {
        __m128 tmpVec = _mm_dp_ps(m_sse, m_sse, 0xff);
        tmpVec = _mm_rsqrt_ps(tmpVec);
        return _mm_mul_ps(m_sse, tmpVec);
    }

    float Vector<4, float>::Length3Slow() const
    {
        return ::sqrt(Length3Sqr());
    }

    float Vector<4, float>::Length3Sqr() const
    {
        __m128 tmpVec = _mm_dp_ps(m_sse, m_sse, 0x7f);
        _declspec(align(16)) float tmpArr[4];
        _mm_store_ps(tmpArr, tmpVec);
        return tmpArr[0];
    }

    Vector<4, float> Vector<4, float>::Normalize3() const
    {
        __m128 tmpVec = _mm_dp_ps(m_sse, m_sse, 0x7f);
        tmpVec = _mm_rsqrt_ps(tmpVec);
        return _mm_mul_ps(m_sse, tmpVec);
    }

    Vector<4, float> Vector<4, float>::Min(const Vector<4, float>& other) const
    {
        return _mm_min_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::Select(const thisType& other, const thisType& mask) const
    {
        return _mm_blendv_ps(m_sse, other.m_sse, mask.m_sse);
    }

    Vector<4, float> Vector<4, float>::Max(const thisType& other) const
    {
        return _mm_max_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::operator-() const
    {
        _declspec(align(16)) float tmpArr[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
        __m128 tmpVec = _mm_load_ps(tmpArr);
        return _mm_mul_ps(m_sse, tmpVec);
    }

    Vector<4, float>& Vector<4, float>::operator*=(float other)
    {
        *this = *this * other;
        return *this;
    }

    Vector<4, float>& Vector<4, float>::operator/=(float other)
    {
        *this = *this / other;
        return *this;
    }

    Vector<4, float> Vector<4, float>::operator*(float other) const
    {
        _declspec(align(16)) float tmpArr[4] = {other, other, other, other};
        __m128 tmpVec = _mm_load_ps(tmpArr);
        return _mm_mul_ps(m_sse, tmpVec);
    }

    Vector<4, float> Vector<4, float>::operator/(float other) const
    {
        _declspec(align(16)) float tmpArr[4] = {other, other, other, other};
        __m128 tmpVec = _mm_load_ps(tmpArr);
        return _mm_div_ps(m_sse, tmpVec);
    }

    Vector<4, float>& Vector<4, float>::operator*=(const thisType& other)
    {
        m_sse = _mm_mul_ps(m_sse, other.m_sse);
        return *this;
    }

    Vector<4, float>& Vector<4, float>::operator/=(const thisType& other)
    {
        m_sse = _mm_div_ps(m_sse, other.m_sse);
        return *this;
    }

    Vector<4, float>& Vector<4, float>::operator+=(const thisType& other)
    {
        m_sse = _mm_add_ps(m_sse, other.m_sse);
        return *this;
    }

    Vector<4, float>& Vector<4, float>::operator-=(const thisType& other)
    {
        m_sse = _mm_sub_ps(m_sse, other.m_sse);
        return *this;
    }

    Vector<4, float> Vector<4, float>::operator*(const thisType& other) const
    {
        return _mm_mul_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::operator/(const thisType& other) const
    {
        return _mm_div_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::operator+(const thisType& other) const
    {
        return _mm_add_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::operator-(const thisType& other) const
    {
        return _mm_sub_ps(m_sse, other.m_sse);
    }

    bool Vector<4, float>::operator!=(const thisType& other) const
    {
        return !operator==(other);
    }

    bool Vector<4, float>::operator==(const thisType& other) const
    {
        __m128 tmpVec = _mm_cmpeq_ps(m_sse, other.m_sse);
        int mask = _mm_movemask_ps(tmpVec);
        return mask == 0xf;
    }

    Vector<4, float> Vector<4, float>::operator>(const thisType& other) const
    {
        return _mm_cmpgt_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::operator<(const thisType& other) const
    {
        return _mm_cmplt_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::operator>=(const thisType& other) const
    {
        return _mm_cmpge_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::operator<=(const thisType& other) const
    {
        return _mm_cmple_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::EqualMask(const thisType& other) const
    {
        return _mm_cmpeq_ps(m_sse, other.m_sse);
    }

    Vector<4, float> Vector<4, float>::NotEqualMask(const thisType& other) const
    {
        return _mm_cmpneq_ps(m_sse, other.m_sse);
    }

} // namespace Boolka

#endif
