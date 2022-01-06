#pragma once
#include "Vector.h"

namespace Boolka
{

    // Row major
    class [[nodiscard]] Matrix4x4
    {
    public:
        Matrix4x4();
        ~Matrix4x4();

        Matrix4x4(const Matrix4x4&) = default;
        Matrix4x4(Matrix4x4&&) = default;
        Matrix4x4& operator=(const Matrix4x4&) = default;
        Matrix4x4& operator=(Matrix4x4&&) = default;

        Matrix4x4(const Vector4& row1, const Vector4& row2, const Vector4& row3,
                  const Vector4& row4);
        Matrix4x4(std::initializer_list<float> list);

        Vector4& operator[](size_t i);
        const Vector4& operator[](size_t i) const;

        static_assert(sizeof(Vector4) == (sizeof(float) * 4),
                      "GetBuffer assumes contiguous Vector4's form contiguous array of floats");
        [[nodiscard]] float* GetBuffer();
        [[nodiscard]] const float* GetBuffer() const;

        [[nodiscard]] Vector4* begin();
        [[nodiscard]] Vector4* end();
        [[nodiscard]] const Vector4* begin() const;
        [[nodiscard]] const Vector4* end() const;

        [[nodiscard]] Matrix4x4 operator-();

        Matrix4x4& operator*=(float other);
        Matrix4x4& operator/=(float other);
        [[nodiscard]] Matrix4x4 operator*(float other) const;
        [[nodiscard]] Matrix4x4 operator/(float other) const;

        [[nodiscard]] Matrix4x4 operator*(const Matrix4x4& other) const;

        [[nodiscard]] bool operator==(const Matrix4x4& other) const;
        [[nodiscard]] bool operator!=(const Matrix4x4& other) const;

        [[nodiscard]] Matrix4x4 Inverse(bool& isSuccessfull) const;
        [[nodiscard]] Matrix4x4 Transpose() const;

        [[nodiscard]] static Matrix4x4 GetIdentity();
        [[nodiscard]] static Matrix4x4 GetTranslation(float x, float y, float z);
        [[nodiscard]] static Matrix4x4 GetTranslation(const Vector4& xyz);
        [[nodiscard]] static Matrix4x4 GetScale(float uniformScale);
        [[nodiscard]] static Matrix4x4 GetScale(float x, float y, float z);
        [[nodiscard]] static Matrix4x4 GetRotationX(float angle);
        [[nodiscard]] static Matrix4x4 GetRotationY(float angle);
        [[nodiscard]] static Matrix4x4 GetRotationZ(float angle);

        [[nodiscard]] static Matrix4x4 CalculateView(const Vector4& right, const Vector4& up,
                                                     const Vector4& forward,
                                                     const Vector4& position);
        [[nodiscard]] static Matrix4x4 CalculateProjPerspective(float nearZ, float farZ,
                                                                float aspectRatio, float fovY);
        [[nodiscard]] static Matrix4x4 CalculateProjOrtographic(float nearZ, float farZ,
                                                                float width, float height);

        // Matrix that transforms UV (-1..1 range) to texcoords(0..1 range) and backwards
        [[nodiscard]] static Matrix4x4 GetUVToTexCoord();
        [[nodiscard]] static Matrix4x4 GetTexCoordToUV();

        [[nodiscard]] static Matrix4x4 CalculateCubeMapView(size_t cubeMapFace,
                                                            const Vector4& position);

    private:
        Vector4 m_data[4];
    };

    [[nodiscard]] Vector4 operator*(const Vector4& first, const Matrix4x4& second);

} // namespace Boolka