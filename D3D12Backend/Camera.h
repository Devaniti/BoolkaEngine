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

        bool Camera::Update(float deltaTime, float aspectRatio, Matrix4x4& outViewMatrix, Matrix4x4& outProjMatrix);

    private:
        float m_RotationYaw;
        float m_RotationPitch;
        Vector4 m_CameraPos;
    };

}
