#pragma once

namespace Boolka
{

    class Camera
    {
    public:
        Camera();
        ~Camera();

        bool Initialize(float rotationYaw, float rotationPitch, const Vector4& cameraPos);
        void Unload();

        bool Update(float deltaTime, float aspectRatio, Matrix4x4& outViewMatrix, Matrix4x4& outProjMatrix, Vector3& outCameraPos);

    private:

        void UpdateInput(float deltaTime, const Vector3& right, const Vector3& up, const Vector3& forward);
        void UpdateMatrices(float aspectRatio, const Vector3& right, const Vector3& up, const Vector3& forward, Matrix4x4& outViewMatrix, Matrix4x4& outProjMatrix);
        float m_RotationYaw;
        float m_RotationPitch;
        Vector4 m_CameraPos;
    };

}
