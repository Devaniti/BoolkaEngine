#include "stdafx.h"
#include "Vector.h"

namespace Boolka
{

    float Vector4::Dot(const Vector4& other) const
    {
        float result = 0;
        for (size_t i = 0; i < 4; i++)
        {
            result += (*this)[i] * other[i];
        }

        return result;
    }

    Vector4 Vector4::Cross3(const Vector4& other) const
    {
        return Vector4
        {
            (*this)[1] * other[2] - (*this)[2] * other[1],
            (*this)[2] * other[0] - (*this)[0] * other[2],
            (*this)[0] * other[1] - (*this)[1] * other[0],
            0.0f
        };
    }

    float Vector4::Length() const
    {
        return ::sqrt(LengthSqr());
    }

    float Vector4::LengthSqr() const
    {
        float result = 0;
        for (auto& value : m_data)
        {
            result += value * value;
        }

        return result;
    }

    Vector4 Vector4::Normalize() const
    {
        return (*this) / Length();
    }

    Vector4& Vector4::operator*=(float other)
    {
        for (float& element : m_data)
        {
            element *= other;
        }

        return *this;
    }

    Vector4& Vector4::operator/=(float other)
    {
        for (float& element : m_data)
        {
            element /= other;
        }

        return *this;
    }

    Vector4 Vector4::operator*(float other) const
    {
        Vector4 result = *this;
        result *= other;
        return result;
    }

    Vector4 Vector4::operator/(float other) const
    {
        Vector4 result = *this;
        result /= other;
        return result;
    }

    Vector4& Vector4::operator*=(const Vector4& other)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            (*this)[i] *= other[i];
        }
        return *this;
    }

    Vector4& Vector4::operator/=(const Vector4& other)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            (*this)[i] /= other[i];
        }
        return *this;
    }

    Vector4& Vector4::operator+=(const Vector4& other)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            (*this)[i] += other[i];
        }
        return *this;
    }

    Vector4& Vector4::operator-=(const Vector4& other)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            (*this)[i] -= other[i];
        }
        return *this;
    }

    Vector4 Vector4::operator-() const
    {
        return Vector4{ -m_data[0], -m_data[1], -m_data[2], -m_data[3] };
    }

    Vector4 Vector4::operator*(const Vector4& other) const
    {
        Vector4 result = *this;
        result *= other;
        return result;
    }

    Vector4 Vector4::operator/(const Vector4& other) const
    {
        Vector4 result = *this;
        result /= other;
        return result;
    }

    Vector4 Vector4::operator+(const Vector4& other) const
    {
        Vector4 result = *this;
        result += other;
        return result;
    }

    Vector4 Vector4::operator-(const Vector4& other) const
    {
        Vector4 result = *this;
        result -= other;
        return result;
    }

    bool Vector4::operator==(const Vector4& other) const
    {
        return std::equal(std::begin(m_data), std::end(m_data), std::begin(other.m_data));
    }

    bool Vector4::operator!=(const Vector4& other) const
    {
        return !operator==(other);
    }

}
