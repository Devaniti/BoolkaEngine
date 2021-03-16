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
            vector = Vector4(iter, iter + 4);
            iter += 4;
        }
    }

    Matrix4x4::Matrix4x4(const Vector4& row1, const Vector4& row2, const Vector4& row3,
                         const Vector4& row4)
        : m_data{row1, row2, row3, row4}
    {
    }

    Matrix4x4::Matrix4x4()
    {
    }

    Matrix4x4::~Matrix4x4()
    {
    }

    const Vector4* Matrix4x4::end() const
    {
        return m_data + 4;
    }

    Vector4* Matrix4x4::end()
    {
        return m_data + 4;
    }

    const Vector4& Matrix4x4::operator[](size_t i) const
    {
        BLK_ASSERT(i < 4);
        return m_data[i];
    }

    Vector4& Matrix4x4::operator[](size_t i)
    {
        BLK_ASSERT(i < 4);
        return m_data[i];
    }

    const float* Matrix4x4::GetBuffer() const
    {
        return m_data[0].GetBuffer();
    }

    float* Matrix4x4::GetBuffer()
    {
        return m_data[0].GetBuffer();
    }

    const Vector4* Matrix4x4::begin() const
    {
        return m_data;
    }

    Vector4* Matrix4x4::begin()
    {
        return m_data;
    }

    Matrix4x4 Matrix4x4::operator-()
    {
        Matrix4x4 result;
        for (size_t i = 0; i < 4; i++)
        {
            result[i] = -m_data[i];
        }
        return result;
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
        // Initialized with zeroes
        Matrix4x4 result;

        // Order is important for cache coherence
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                result[i] += other[j] * (*this)[i][j];
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

    Matrix4x4 Matrix4x4::Inverse(bool& isSuccessfull) const
    {
        Matrix4x4 result;

        result[0][0] = (*this)[1][1] * (*this)[2][2] * (*this)[3][3] -
                       (*this)[1][1] * (*this)[2][3] * (*this)[3][2] -
                       (*this)[2][1] * (*this)[1][2] * (*this)[3][3] +
                       (*this)[2][1] * (*this)[1][3] * (*this)[3][2] +
                       (*this)[3][1] * (*this)[1][2] * (*this)[2][3] -
                       (*this)[3][1] * (*this)[1][3] * (*this)[2][2];

        result[1][0] = -(*this)[1][0] * (*this)[2][2] * (*this)[3][3] +
                       (*this)[1][0] * (*this)[2][3] * (*this)[3][2] +
                       (*this)[2][0] * (*this)[1][2] * (*this)[3][3] -
                       (*this)[2][0] * (*this)[1][3] * (*this)[3][2] -
                       (*this)[3][0] * (*this)[1][2] * (*this)[2][3] +
                       (*this)[3][0] * (*this)[1][3] * (*this)[2][2];

        result[2][0] = (*this)[1][0] * (*this)[2][1] * (*this)[3][3] -
                       (*this)[1][0] * (*this)[2][3] * (*this)[3][1] -
                       (*this)[2][0] * (*this)[1][1] * (*this)[3][3] +
                       (*this)[2][0] * (*this)[1][3] * (*this)[3][1] +
                       (*this)[3][0] * (*this)[1][1] * (*this)[2][3] -
                       (*this)[3][0] * (*this)[1][3] * (*this)[2][1];

        result[3][0] = -(*this)[1][0] * (*this)[2][1] * (*this)[3][2] +
                       (*this)[1][0] * (*this)[2][2] * (*this)[3][1] +
                       (*this)[2][0] * (*this)[1][1] * (*this)[3][2] -
                       (*this)[2][0] * (*this)[1][2] * (*this)[3][1] -
                       (*this)[3][0] * (*this)[1][1] * (*this)[2][2] +
                       (*this)[3][0] * (*this)[1][2] * (*this)[2][1];

        result[0][1] = -(*this)[0][1] * (*this)[2][2] * (*this)[3][3] +
                       (*this)[0][1] * (*this)[2][3] * (*this)[3][2] +
                       (*this)[2][1] * (*this)[0][2] * (*this)[3][3] -
                       (*this)[2][1] * (*this)[0][3] * (*this)[3][2] -
                       (*this)[3][1] * (*this)[0][2] * (*this)[2][3] +
                       (*this)[3][1] * (*this)[0][3] * (*this)[2][2];

        result[1][1] = (*this)[0][0] * (*this)[2][2] * (*this)[3][3] -
                       (*this)[0][0] * (*this)[2][3] * (*this)[3][2] -
                       (*this)[2][0] * (*this)[0][2] * (*this)[3][3] +
                       (*this)[2][0] * (*this)[0][3] * (*this)[3][2] +
                       (*this)[3][0] * (*this)[0][2] * (*this)[2][3] -
                       (*this)[3][0] * (*this)[0][3] * (*this)[2][2];

        result[2][1] = -(*this)[0][0] * (*this)[2][1] * (*this)[3][3] +
                       (*this)[0][0] * (*this)[2][3] * (*this)[3][1] +
                       (*this)[2][0] * (*this)[0][1] * (*this)[3][3] -
                       (*this)[2][0] * (*this)[0][3] * (*this)[3][1] -
                       (*this)[3][0] * (*this)[0][1] * (*this)[2][3] +
                       (*this)[3][0] * (*this)[0][3] * (*this)[2][1];

        result[3][1] = (*this)[0][0] * (*this)[2][1] * (*this)[3][2] -
                       (*this)[0][0] * (*this)[2][2] * (*this)[3][1] -
                       (*this)[2][0] * (*this)[0][1] * (*this)[3][2] +
                       (*this)[2][0] * (*this)[0][2] * (*this)[3][1] +
                       (*this)[3][0] * (*this)[0][1] * (*this)[2][2] -
                       (*this)[3][0] * (*this)[0][2] * (*this)[2][1];

        result[0][2] = (*this)[0][1] * (*this)[1][2] * (*this)[3][3] -
                       (*this)[0][1] * (*this)[1][3] * (*this)[3][2] -
                       (*this)[1][1] * (*this)[0][2] * (*this)[3][3] +
                       (*this)[1][1] * (*this)[0][3] * (*this)[3][2] +
                       (*this)[3][1] * (*this)[0][2] * (*this)[1][3] -
                       (*this)[3][1] * (*this)[0][3] * (*this)[1][2];

        result[1][2] = -(*this)[0][0] * (*this)[1][2] * (*this)[3][3] +
                       (*this)[0][0] * (*this)[1][3] * (*this)[3][2] +
                       (*this)[1][0] * (*this)[0][2] * (*this)[3][3] -
                       (*this)[1][0] * (*this)[0][3] * (*this)[3][2] -
                       (*this)[3][0] * (*this)[0][2] * (*this)[1][3] +
                       (*this)[3][0] * (*this)[0][3] * (*this)[1][2];

        result[2][2] = (*this)[0][0] * (*this)[1][1] * (*this)[3][3] -
                       (*this)[0][0] * (*this)[1][3] * (*this)[3][1] -
                       (*this)[1][0] * (*this)[0][1] * (*this)[3][3] +
                       (*this)[1][0] * (*this)[0][3] * (*this)[3][1] +
                       (*this)[3][0] * (*this)[0][1] * (*this)[1][3] -
                       (*this)[3][0] * (*this)[0][3] * (*this)[1][1];

        result[3][2] = -(*this)[0][0] * (*this)[1][1] * (*this)[3][2] +
                       (*this)[0][0] * (*this)[1][2] * (*this)[3][1] +
                       (*this)[1][0] * (*this)[0][1] * (*this)[3][2] -
                       (*this)[1][0] * (*this)[0][2] * (*this)[3][1] -
                       (*this)[3][0] * (*this)[0][1] * (*this)[1][2] +
                       (*this)[3][0] * (*this)[0][2] * (*this)[1][1];

        result[0][3] = -(*this)[0][1] * (*this)[1][2] * (*this)[2][3] +
                       (*this)[0][1] * (*this)[1][3] * (*this)[2][2] +
                       (*this)[1][1] * (*this)[0][2] * (*this)[2][3] -
                       (*this)[1][1] * (*this)[0][3] * (*this)[2][2] -
                       (*this)[2][1] * (*this)[0][2] * (*this)[1][3] +
                       (*this)[2][1] * (*this)[0][3] * (*this)[1][2];

        result[1][3] = (*this)[0][0] * (*this)[1][2] * (*this)[2][3] -
                       (*this)[0][0] * (*this)[1][3] * (*this)[2][2] -
                       (*this)[1][0] * (*this)[0][2] * (*this)[2][3] +
                       (*this)[1][0] * (*this)[0][3] * (*this)[2][2] +
                       (*this)[2][0] * (*this)[0][2] * (*this)[1][3] -
                       (*this)[2][0] * (*this)[0][3] * (*this)[1][2];

        result[2][3] = -(*this)[0][0] * (*this)[1][1] * (*this)[2][3] +
                       (*this)[0][0] * (*this)[1][3] * (*this)[2][1] +
                       (*this)[1][0] * (*this)[0][1] * (*this)[2][3] -
                       (*this)[1][0] * (*this)[0][3] * (*this)[2][1] -
                       (*this)[2][0] * (*this)[0][1] * (*this)[1][3] +
                       (*this)[2][0] * (*this)[0][3] * (*this)[1][1];

        result[3][3] = (*this)[0][0] * (*this)[1][1] * (*this)[2][2] -
                       (*this)[0][0] * (*this)[1][2] * (*this)[2][1] -
                       (*this)[1][0] * (*this)[0][1] * (*this)[2][2] +
                       (*this)[1][0] * (*this)[0][2] * (*this)[2][1] +
                       (*this)[2][0] * (*this)[0][1] * (*this)[1][2] -
                       (*this)[2][0] * (*this)[0][2] * (*this)[1][1];

        float det = (*this)[0][0] * result[0][0] + (*this)[0][1] * result[1][0] +
                    (*this)[0][2] * result[2][0] + (*this)[0][3] * result[3][0];

        if (::abs(det) < FLT_EPSILON)
        {
            isSuccessfull = false;
            return result;
        }

        isSuccessfull = true;

        result /= det;

        return result;
    }

#ifdef BLK_USE_SSE

    Matrix4x4 Matrix4x4::Transpose() const
    {
        // Same algorithm as in _MM_TRANSPOSE4_PS

        const __m128& row0 = m_data[0].GetInternal();
        const __m128& row1 = m_data[1].GetInternal();
        const __m128& row2 = m_data[2].GetInternal();
        const __m128& row3 = m_data[3].GetInternal();
        __m128 tmp3, tmp2, tmp1, tmp0;

        tmp0 = _mm_shuffle_ps((row0), (row1), 0x44);
        tmp2 = _mm_shuffle_ps((row0), (row1), 0xEE);
        tmp1 = _mm_shuffle_ps((row2), (row3), 0x44);
        tmp3 = _mm_shuffle_ps((row2), (row3), 0xEE);

        return Matrix4x4{
            _mm_shuffle_ps(tmp0, tmp1, 0x88),
            _mm_shuffle_ps(tmp0, tmp1, 0xDD),
            _mm_shuffle_ps(tmp2, tmp3, 0x88),
            _mm_shuffle_ps(tmp2, tmp3, 0xDD),
        };
    }

#else

    Matrix4x4 Matrix4x4::Transpose() const
    {
        return Matrix4x4{
            {(*this)[0][0], (*this)[1][0], (*this)[2][0], (*this)[3][0]},
            {(*this)[0][1], (*this)[1][1], (*this)[2][1], (*this)[3][1]},
            {(*this)[0][2], (*this)[1][2], (*this)[2][2], (*this)[3][2]},
            {(*this)[0][3], (*this)[1][3], (*this)[2][3], (*this)[3][3]},
        };
    }

#endif // BLK_USE_SSE

    Matrix4x4 Matrix4x4::GetIdentity()
    {
        return Matrix4x4{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    Matrix4x4 Matrix4x4::GetTranslation(float x, float y, float z)
    {
        return Matrix4x4{
            {1.0f, 0.0f, 0.0f, x},
            {0.0f, 1.0f, 0.0f, y},
            {0.0f, 0.0f, 1.0f, z},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    Matrix4x4 Matrix4x4::GetTranslation(const Vector4& xyz)
    {
        return Matrix4x4{
            {1.0f, 0.0f, 0.0f, xyz[0]},
            {0.0f, 1.0f, 0.0f, xyz[1]},
            {0.0f, 0.0f, 1.0f, xyz[2]},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    Matrix4x4 Matrix4x4::GetScale(float uniformScale)
    {
        return Matrix4x4{
            {uniformScale, 0.0f, 0.0f, 0.0f},
            {0.0f, uniformScale, 0.0f, 0.0f},
            {0.0f, 0.0f, uniformScale, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    Matrix4x4 Matrix4x4::GetScale(float x, float y, float z)
    {
        return Matrix4x4{
            {x, 0.0f, 0.0f, 0.0f},
            {0.0f, y, 0.0f, 0.0f},
            {0.0f, 0.0f, z, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    Matrix4x4 Matrix4x4::GetRotationX(float angle)
    {
        return Matrix4x4{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, cos(angle), -sin(angle), 0.0f},
            {0.0f, sin(angle), cos(angle), 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    Matrix4x4 Matrix4x4::GetRotationY(float angle)
    {
        return Matrix4x4{
            {cos(angle), 0.0f, sin(angle), 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {-sin(angle), 0.0f, cos(angle), 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    Matrix4x4 Matrix4x4::GetRotationZ(float angle)
    {
        return Matrix4x4{
            {cos(angle), -sin(angle), 0.0f, 0.0f},
            {sin(angle), cos(angle), 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
    }

    Matrix4x4 Matrix4x4::CalculateView(const Vector4& right, const Vector4& up,
                                       const Vector4& forward, const Vector4& position)
    {
        const Vector4 minusPos = -position;

        Matrix4x4 view{
            {right[0], up[0], forward[0], 0.0f},
            {right[1], up[1], forward[1], 0.0f},
            {right[2], up[2], forward[2], 0.0f},
            {minusPos.Dot(right), minusPos.Dot(up), minusPos.Dot(forward), 1.0f},
        };

        return view;
    }

    Matrix4x4 Matrix4x4::CalculateCubeMapView(size_t cubeMapFace, const Vector4& position)
    {
        BLK_ASSERT(cubeMapFace < 6);

        static const Vector4 right[6] = {
            {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
        };

        static const Vector4 up[6] = {
            {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
        };

        static const Vector4 forward[6] = {
            {1.0f, 0.0f, 0.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
            {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, -1.0f},
        };

        return CalculateView(right[cubeMapFace], up[cubeMapFace], forward[cubeMapFace], position);
    }

    Matrix4x4 Matrix4x4::CalculateProjPerspective(float nearZ, float farZ, float aspectRatio,
                                                  float fovY)
    {
        BLK_ASSERT(farZ > nearZ);

        float h = 1.0f / tan(fovY * 0.5f);
        float w = h / aspectRatio;
        float a = farZ / (farZ - nearZ);
        float b = (-nearZ * farZ) / (farZ - nearZ);

        Matrix4x4 projMatrix = {
            {w, 0, 0, 0},
            {0, h, 0, 0},
            {0, 0, a, 1},
            {0, 0, b, 0},
        };

        return projMatrix;
    }

    Matrix4x4 Matrix4x4::CalculateProjOrtographic(float nearZ, float farZ, float width,
                                                  float height)
    {
        BLK_ASSERT(farZ > nearZ);

        float w = 2.0f / width;
        float h = 2.0f / height;
        float a = 1.0f / (farZ - nearZ);
        float b = -a * nearZ;

        Matrix4x4 projMatrix = {
            {w, 0, 0, 0},
            {0, h, 0, 0},
            {0, 0, a, 0},
            {0, 0, b, 1},
        };

        return projMatrix;
    }

    Matrix4x4 Matrix4x4::GetUVToTexCoord()
    {
        return Matrix4x4{
            {0.5f, 0, 0, 0},
            {0, -0.5f, 0, 0},
            {0, 0, 1, 0},
            {0.5f, 0.5f, 0, 1},
        };
    }

    Matrix4x4 Matrix4x4::GetTexCoordToUV()
    {
        return Matrix4x4{
            {2.0f, 0, 0, 0},
            {0, -2.0f, 0, 0},
            {0, 0, 1, 0},
            {-1.0f, 1.0f, 0, 1},
        };
    }

    Vector4 operator*(const Vector4& first, const Matrix4x4& second)
    {
        Vector4 result;

        for (size_t i = 0; i < 4; i++)
        {
            result += second[i] * first[i];
        }

        return result;
    }

} // namespace Boolka
