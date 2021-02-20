#pragma once
#include "Structures/Vector.h"

namespace Boolka
{

    // Row major
    class Matrix4x4
    {
    public:
        Matrix4x4() {};
        ~Matrix4x4() {};

        Matrix4x4(const Matrix4x4&) = default;
        Matrix4x4(Matrix4x4&&) = default;
        Matrix4x4& operator=(const Matrix4x4&) = default;
        Matrix4x4& operator=(Matrix4x4&&) = default;

        Matrix4x4(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4) : m_data{ row1, row2, row3, row4 } {};
        Matrix4x4(std::initializer_list<float> list);

        Vector4& operator[](size_t i) { BLK_ASSERT(i < 4); return m_data[i]; };
        const Vector4& operator[](size_t i) const { BLK_ASSERT(i < 4); return m_data[i]; };

        static_assert(sizeof(Vector4) == (sizeof(float) * 4), "GetBuffer assumes contiguous Vector4's form contiguous array of floats");
        float* GetBuffer() { return m_data[0].GetBuffer(); };
        const float* GetBuffer() const { return m_data[0].GetBuffer(); };

        Vector4* begin() { return m_data; }
        Vector4* end() { return m_data + 4; }
        const Vector4* begin() const { return m_data; }
        const Vector4* end() const { return m_data + 4; }

        Matrix4x4 operator-();

        Matrix4x4& operator*=(float other);
        Matrix4x4& operator/=(float other);
        Matrix4x4 operator*(float other) const;
        Matrix4x4 operator/(float other) const;

        Matrix4x4 operator*(const Matrix4x4& other) const;

        bool operator==(const Matrix4x4& other) const;
        bool operator!=(const Matrix4x4& other) const;

        Matrix4x4 Inverse(bool& isSuccessfull) const;
        Matrix4x4 Transpose() const;

        static Matrix4x4 GetIdentity();
        static Matrix4x4 GetTranslation(float x, float y, float z);
        static Matrix4x4 GetTranslation(const Vector4& xyz);
        static Matrix4x4 GetScale(float uniformScale);
        static Matrix4x4 GetScale(float x, float y, float z);
        static Matrix4x4 GetRotationX(float angle);
        static Matrix4x4 GetRotationY(float angle);
        static Matrix4x4 GetRotationZ(float angle);
        static Matrix4x4 GetView(const Vector4& right, const Vector4& up, const Vector4& forward, const Vector4& position);

    private:
        Vector4 m_data[4];
    };

    Vector4 operator*(const Vector4& first, const Matrix4x4& second);

}