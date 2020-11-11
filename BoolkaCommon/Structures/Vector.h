#pragma once

namespace Boolka
{

    class Vector4
    {
    public:
        Vector4() : m_data{} {};
        ~Vector4() {};

        Vector4(const Vector4&) = default;
        Vector4(Vector4&&) = default;
        Vector4& operator=(const Vector4&) = default;
        Vector4& operator=(Vector4&&) = default;

        Vector4(float x, float y, float z, float w) : m_data{ x,y,z,w } {};

        float x() const { return m_data[0]; };
        float y() const { return m_data[1]; };
        float z() const { return m_data[2]; };
        float w() const { return m_data[3]; };

        float& operator[](size_t i) { BLK_ASSERT(i < 4); return m_data[i]; };
        const float& operator[](size_t i) const { BLK_ASSERT(i < 4); return m_data[i]; };

        float* GetBuffer() { return m_data; }
        const float* GetBuffer() const { return m_data; }

        float* begin() { return m_data; }
        float* end() { return m_data + 4; }
        const float* begin() const { return m_data; }
        const float* end() const { return m_data + 4; }

        float Dot(const Vector4& other) const;
        Vector4 Cross3(const Vector4& other) const;
        float Length() const;
        float LengthSqr() const;

        Vector4 Normalize() const;

        Vector4 operator-() const;

        Vector4& operator*=(float other);
        Vector4& operator/=(float other);
        Vector4 operator*(float other) const;
        Vector4 operator/(float other) const;

        Vector4& operator*=(const Vector4& other);
        Vector4& operator/=(const Vector4& other);
        Vector4& operator+=(const Vector4& other);
        Vector4& operator-=(const Vector4& other);
        Vector4 operator*(const Vector4& other) const;
        Vector4 operator/(const Vector4& other) const;
        Vector4 operator+(const Vector4& other) const;
        Vector4 operator-(const Vector4& other) const;

        bool operator==(const Vector4& other) const;
        bool operator!=(const Vector4& other) const;

    private:
        float m_data[4];
    };

}
