#pragma once

namespace Boolka
{

    template<size_t componentCount = 4, typename elementType = float>
    class Vector
    {
    public:
        using thisType = Vector<componentCount, elementType>;

        Vector() : m_data{} {};
        ~Vector() = default;

        Vector(const Vector&) = default;
        Vector(Vector&&) = default;
        Vector& operator=(const Vector&) = default;
        Vector& operator=(Vector&&) = default;

        Vector(std::initializer_list<elementType> data)
            : m_data{}
        {
            BLK_ASSERT(data.size() <= componentCount);
            std::copy(data.begin(), data.end(), begin());
        };

        template<size_t otherComponentCount>
        Vector(const Vector<otherComponentCount, elementType>& other)
            : m_data{}
        {
            if constexpr (otherComponentCount > componentCount)
            {
                std::copy(other.begin(), other.begin() + componentCount, begin());
            }
            else
            {
                std::copy(other.begin(), other.end(), begin());
            }
        }

        elementType x() const { static_assert(componentCount > 0); return m_data[0]; };
        elementType y() const { static_assert(componentCount > 1); return m_data[1]; };
        elementType z() const { static_assert(componentCount > 2); return m_data[2]; };
        elementType w() const { static_assert(componentCount > 3); return m_data[3]; };

        elementType& x() { static_assert(componentCount > 0); return m_data[0]; };
        elementType& y() { static_assert(componentCount > 1); return m_data[1]; };
        elementType& z() { static_assert(componentCount > 2); return m_data[2]; };
        elementType& w() { static_assert(componentCount > 3); return m_data[3]; };

        elementType r() const { static_assert(componentCount > 0); return m_data[0]; };
        elementType g() const { static_assert(componentCount > 1); return m_data[1]; };
        elementType b() const { static_assert(componentCount > 2); return m_data[2]; };
        elementType a() const { static_assert(componentCount > 3); return m_data[3]; };

        elementType& r() { static_assert(componentCount > 0); return m_data[0]; };
        elementType& g() { static_assert(componentCount > 1); return m_data[1]; };
        elementType& b() { static_assert(componentCount > 2); return m_data[2]; };
        elementType& a() { static_assert(componentCount > 3); return m_data[3]; };

        elementType& operator[](size_t i) { BLK_ASSERT(i < componentCount); return m_data[i]; };
        const elementType& operator[](size_t i) const { BLK_ASSERT(i < componentCount); return m_data[i]; };

        elementType* GetBuffer() { return m_data; }
        const elementType* GetBuffer() const { return m_data; }

        elementType* begin() { return m_data; }
        elementType* end() { return m_data + componentCount; }
        const elementType* begin() const { return m_data; }
        const elementType* end() const { return m_data + componentCount; }

        size_t size() const { return componentCount; }

        elementType Dot(const thisType& other) const;
        thisType Cross(const thisType& other) const;
        elementType LengthSlow() const;
        elementType LengthSqr() const;

        thisType Normalize() const;

        elementType Length3Slow() const;
        elementType LengthSqr3() const;
        thisType Normalize3() const;

        thisType operator-() const;

        thisType& operator*=(elementType other);
        thisType& operator/=(elementType other);
        thisType operator*(elementType other) const;
        thisType operator/(elementType other) const;

        thisType& operator*=(const thisType& other);
        thisType& operator/=(const thisType& other);
        thisType& operator+=(const thisType& other);
        thisType& operator-=(const thisType& other);
        thisType operator*(const thisType& other) const;
        thisType operator/(const thisType& other) const;
        thisType operator+(const thisType& other) const;
        thisType operator-(const thisType& other) const;

        bool operator==(const thisType& other) const;
        bool operator!=(const thisType& other) const;


    protected:
        elementType m_data[componentCount];
    };

    template<size_t componentCount, typename elementType>
    elementType Vector<componentCount, elementType>::Dot(const thisType& other) const
    {
        elementType result = 0;
        for (size_t i = 0; i < componentCount; i++)
        {
            result += (*this)[i] * other[i];
        }

        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::Cross(const thisType& other) const
    {
        static_assert(componentCount >= 3);

        return thisType
        {
            (*this)[1] * other[2] - (*this)[2] * other[1],
            (*this)[2] * other[0] - (*this)[0] * other[2],
            (*this)[0] * other[1] - (*this)[1] * other[0]
        };
    }

    template<size_t componentCount, typename elementType>
    elementType Vector<componentCount, elementType>::LengthSlow() const
    {
        return ::sqrt(LengthSqr());
    }

    template<size_t componentCount, typename elementType>
    elementType Vector<componentCount, elementType>::LengthSqr() const
    {
        elementType result = 0;
        for (auto& value : m_data)
        {
            result += value * value;
        }

        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::Normalize() const
    {
        return (*this) / LengthSlow();
    }

    template<size_t componentCount, typename elementType>
    elementType Boolka::Vector<componentCount, elementType>::Length3Slow() const
    {
        static_assert(componentCount >= 3);
        return ::sqrt(LengthSqr3());
    }

    template<size_t componentCount, typename elementType>
    elementType Boolka::Vector<componentCount, elementType>::LengthSqr3() const
    {
        static_assert(componentCount >= 3);
        elementType result = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            result += m_data[i] * m_data[i];
        }

        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Boolka::Vector<componentCount, elementType>::Normalize3() const
    {
        static_assert(componentCount >= 3);
        return (*this) / Length3Slow();
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::operator-() const
    {
        thisType result;
        for (size_t i = 0; i < componentCount; i++)
        {
            result[i] = -(*this)[i];
        }

        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType>& Vector<componentCount, elementType>::operator*=(elementType other)
    {
        for (elementType& element : m_data)
        {
            element *= other;
        }

        return *this;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType>& Vector<componentCount, elementType>::operator/=(elementType other)
    {
        for (float& element : m_data)
        {
            element /= other;
        }

        return *this;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::operator*(elementType other) const
    {
        thisType result = *this;
        result *= other;
        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::operator/(elementType other) const
    {
        thisType result = *this;
        result /= other;
        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType>& Vector<componentCount, elementType>::operator*=(const thisType& other)
    {
        for (size_t i = 0; i < componentCount; ++i)
        {
            (*this)[i] *= other[i];
        }
        return *this;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType>& Vector<componentCount, elementType>::operator/=(const thisType& other)
    {
        for (size_t i = 0; i < componentCount; ++i)
        {
            (*this)[i] /= other[i];
        }
        return *this;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType>& Vector<componentCount, elementType>::operator+=(const thisType& other)
    {
        for (size_t i = 0; i < componentCount; ++i)
        {
            (*this)[i] += other[i];
        }
        return *this;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType>& Vector<componentCount, elementType>::operator-=(const thisType& other)
    {
        for (size_t i = 0; i < componentCount; ++i)
        {
            (*this)[i] -= other[i];
        }
        return *this;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::operator*(const thisType& other) const
    {
        thisType result = *this;
        result *= other;
        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::operator/(const thisType& other) const
    {
        thisType result = *this;
        result /= other;
        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::operator+(const thisType& other) const
    {
        thisType result = *this;
        result += other;
        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Vector<componentCount, elementType>::operator-(const thisType& other) const
    {
        thisType result = *this;
        result -= other;
        return result;
    }

    template<size_t componentCount, typename elementType>
    bool Vector<componentCount, elementType>::operator!=(const thisType& other) const
    {
        return !operator==(other);
    }

    template<size_t componentCount, typename elementType>
    bool Vector<componentCount, elementType>::operator==(const thisType& other) const
    {
        return std::equal(std::begin(m_data), std::end(m_data), std::begin(other.m_data));
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Min(const Vector<componentCount, elementType>& first, const Vector<componentCount, elementType>& second)
    {
        Vector<componentCount, elementType> result;
        for (size_t i = 0; i < componentCount; ++i)
        {
            result[i] = min(first[i], second[i]);
        }
        return result;
    }

    template<size_t componentCount, typename elementType>
    Vector<componentCount, elementType> Max(const Vector<componentCount, elementType>& first, const Vector<componentCount, elementType>& second)
    {
        Vector<componentCount, elementType> result;
        for (size_t i = 0; i < componentCount; ++i)
        {
            result[i] = max(first[i], second[i]);
        }
        return result;
    }

    using Vector2 = Vector<2>;
    using Vector3 = Vector<3>;
    using Vector4 = Vector<4>;

}
