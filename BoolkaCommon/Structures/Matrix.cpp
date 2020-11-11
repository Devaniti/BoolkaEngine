#include "stdafx.h"
#include "Matrix.h"

namespace Boolka
{

    Matrix4x4::Matrix4x4(std::initializer_list<float> list)
    {
        BLK_CRITICAL_ASSERT(list.size() == 16);
        auto iter = list.begin();
        for (auto& vector : m_data)
        {
            for (auto& element : vector)
            {
                element = *iter;
                ++iter;
            }
        }
    }

    Matrix4x4& Matrix4x4::operator*=(float other)
    {
        for (auto& element : m_data)
        {
            element *= other;
        }
        return *this;
    }

    Matrix4x4& Matrix4x4::operator/=(float other)
    {
        for (auto& element : m_data)
        {
            element /= other;
        }
        return *this;
    }

    Matrix4x4 Matrix4x4::operator*(float other) const
    {
        Matrix4x4 result = *this;
        result *= other;
        return result;
    }

    Matrix4x4 Matrix4x4::operator/(float other) const
    {
        Matrix4x4 result = *this;
        result /= other;
        return result;
    }

    Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
    {
        Matrix4x4 result;

        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                for (size_t k = 0; k < 4; k++)
                {
                    result[i][k] += (*this)[i][j] * other[j][k];
                }
            }
        }

        return result;
    }

    bool Matrix4x4::operator==(const Matrix4x4& other) const
    {
        return std::equal(std::begin(m_data), std::end(m_data), std::begin(other.m_data));
    }

    bool Matrix4x4::operator!=(const Matrix4x4& other) const
    {
        return !operator==(other);
    }

    Matrix4x4 Matrix4x4::Transpose() const
    {
        return Matrix4x4
        {
            (*this)[0][0], (*this)[1][0], (*this)[2][0], (*this)[3][0],
            (*this)[0][1], (*this)[1][1], (*this)[2][1], (*this)[3][1],
            (*this)[0][2], (*this)[1][2], (*this)[2][2], (*this)[3][2],
            (*this)[0][3], (*this)[1][3], (*this)[2][3], (*this)[3][3],
        };
    }

    Matrix4x4 Matrix4x4::GetIdentity()
    {
        return Matrix4x4
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
    }

    Matrix4x4 Matrix4x4::GetTranslation(float x, float y, float z)
    {
        return Matrix4x4
        {
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
    }

    Matrix4x4 Matrix4x4::GetTranslation(const Vector4& xyz)
    {
        return Matrix4x4
        {
            1.0f, 0.0f, 0.0f, xyz[0],
            0.0f, 1.0f, 0.0f, xyz[1],
            0.0f, 0.0f, 1.0f, xyz[2],
            0.0f, 0.0f, 0.0f, 1.0f,
        };
    }

    Matrix4x4 Matrix4x4::GetScale(float uniformScale)
    {
        return Matrix4x4
        {
            uniformScale,         0.0f,         0.0f, 0.0f,
                    0.0f, uniformScale,         0.0f, 0.0f,
                    0.0f,         0.0f, uniformScale, 0.0f,
                    0.0f,         0.0f,         0.0f, 1.0f,
        };
    }

    Matrix4x4 Matrix4x4::GetScale(float x, float y, float z)
    {
        return Matrix4x4
        {
               x, 0.0f, 0.0f, 0.0f,
            0.0f,    y, 0.0f, 0.0f,
            0.0f, 0.0f,    z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
    }

    Matrix4x4 Matrix4x4::GetRotationX(float angle)
    {
        return Matrix4x4
        {
            1.0f,       0.0f,        0.0f, 0.0f,
            0.0f, cos(angle), -sin(angle), 0.0f,
            0.0f, sin(angle),  cos(angle), 0.0f,
            0.0f,       0.0f,        0.0f, 1.0f,
        };
    }

    Matrix4x4 Matrix4x4::GetRotationY(float angle)
    {
        return Matrix4x4
        {
             cos(angle), 0.0f, sin(angle), 0.0f,
                   0.0f, 1.0f,       0.0f, 0.0f,
            -sin(angle), 0.0f, cos(angle), 0.0f,
                   0.0f, 0.0f,       0.0f, 1.0f,
        };
    }

    Matrix4x4 Matrix4x4::GetRotationZ(float angle)
    {
        return Matrix4x4
        {
            cos(angle), -sin(angle), 0.0f, 0.0f,
            sin(angle),  cos(angle), 0.0f, 0.0f,
                  0.0f,        0.0f, 1.0f, 0.0f,
                  0.0f,        0.0f, 0.0f, 1.0f,
        };
    }

    Matrix4x4 Matrix4x4::GetView(const Vector4& right, const Vector4& up, const Vector4& forward, const Vector4& position)
    {
        Vector4 negativePos = -position;

        //Matrix4x4 view
        //{
        //                  right[0],            right[1],                 right[2], 0.0f,
        //                     up[0],               up[1],                    up[2], 0.0f,
        //                forward[0],          forward[1],               forward[2], 0.0f,
        //    negativePos.Dot(right), negativePos.Dot(up), negativePos.Dot(forward), 1.0f,
        //};

        Matrix4x4 view
        {
                          right[0],               up[0],               forward[0], 0.0f,
                          right[1],               up[1],               forward[1], 0.0f,
                          right[2],               up[2],               forward[2], 0.0f,
            negativePos.Dot(right), negativePos.Dot(up), negativePos.Dot(forward), 1.0f,
        };

        return view;
    }

    Vector4 operator*(const Vector4& first, const Matrix4x4& second)
    {
        Vector4 result;

        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                result[i] += first[j] * second[j][i];
            }
        }

        return result;
    }

}

