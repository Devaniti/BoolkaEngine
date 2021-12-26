#pragma once

namespace Boolka
{

    class Camera
    {
    public:
        Camera();
        ~Camera();

        bool Initialize(float rotationYaw, float rotationPitch, float fieldOfView,
                        const Vector4& cameraPos);
        void Unload();

        bool Update(float deltaTime, float aspectRatio, Matrix4x4& outViewMatrix,
                    Matrix4x4& outProjMatrix, Vector4& outCameraPos, Vector4* outEyeRayCoeficients);

    private:
        void UpdateInput(float deltaTime, const Vector4& right, const Vector4& up,
                         const Vector4& forward);
        void UpdateMatrices(float aspectRatio, const Vector4& right, const Vector4& up,
                            const Vector4& forward, Matrix4x4& outViewMatrix,
                            Matrix4x4& outProjMatrix);
        float m_RotationYaw;
        float m_RotationPitch;
        float m_FieldOfView;
        Vector4 m_CameraPos;
    };

} // namespace Boolka
