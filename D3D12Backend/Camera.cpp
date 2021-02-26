#include "stdafx.h"
#include "Camera.h"

namespace Boolka
{

    Camera::Camera()
        : m_RotationYaw(0.0f)
        , m_RotationPitch(0.0f)
        , m_CameraPos{}
    {
    }

    Camera::~Camera()
    {
        BLK_ASSERT(m_RotationYaw == 0.0f);
        BLK_ASSERT(m_RotationPitch == 0.0f);
        BLK_ASSERT(m_CameraPos == Vector4{});
    }

    bool Camera::Initialize(float rotationYaw, float rotationPitch, const Vector4& cameraPos)
    {
        m_RotationYaw = rotationYaw;
        m_RotationPitch = rotationPitch;
        m_CameraPos = cameraPos;

        return true;
    }

    void Camera::Unload()
    {
        m_RotationYaw = 0.0f;
        m_RotationPitch = 0.0f;
        m_CameraPos = {};
    }

    bool Camera::Update(float deltaTime, float aspectRatio, Matrix4x4& outViewMatrix, Matrix4x4& outProjMatrix, Vector3& outCameraPos)
    {
        static const Vector3 upDirection{ 0, 0, 1 };
        Vector3 forward
        {
            cos(m_RotationYaw) * cos(m_RotationPitch),
            sin(m_RotationYaw) * cos(m_RotationPitch),
            sin(m_RotationPitch)
        };
        Vector3 right = upDirection.Cross(forward).Normalize();
        Vector3 up = forward.Cross(right).Normalize();

        UpdateInput(deltaTime, right, up, forward);
        UpdateMatrices(aspectRatio, right, up, forward, outViewMatrix, outProjMatrix);
        outCameraPos = m_CameraPos;

        return true;
    }

    void Camera::UpdateInput(float deltaTime, const Vector3& right, const Vector3& up, const Vector3& forward)
    {
        static const float defaultMoveSpeed = 15.0f;
        static const float defaultRotationSpeed = BLK_DEG_TO_RAD(60.0f);

        float speedMult = 1.0f;
        float angleSpeedMult = 1.0f;

        if (::GetAsyncKeyState(VK_SHIFT))
        {
            speedMult *= 5.0f;
            angleSpeedMult *= 2.5f;
        }

        if (::GetAsyncKeyState(VK_CONTROL))
        {
            speedMult /= 5.0f;
            angleSpeedMult /= 2.5f;
        }

        float moveDelta = defaultMoveSpeed * deltaTime * speedMult;
        float rotationDelta = defaultRotationSpeed * deltaTime * angleSpeedMult;

        bool leftPressed = static_cast<bool>(::GetAsyncKeyState(VK_LEFT));
        bool rightPressed = static_cast<bool>(::GetAsyncKeyState(VK_RIGHT));
        int rotationYawChange = static_cast<int>(rightPressed) - static_cast<int>(leftPressed);
        if (rotationYawChange != 0)
        {
            m_RotationYaw += rotationDelta * rotationYawChange;
            m_RotationYaw = fmod(m_RotationYaw, 2 * BLK_FLOAT_PI);
        }

        bool upPressed = static_cast<bool>(::GetAsyncKeyState(VK_UP));
        bool downPressed = static_cast<bool>(::GetAsyncKeyState(VK_DOWN));
        int rotationPitchChange = static_cast<int>(upPressed) - static_cast<int>(downPressed);
        if (rotationPitchChange != 0)
        {
            m_RotationPitch += rotationDelta * rotationPitchChange;
            m_RotationPitch = std::clamp(m_RotationPitch, -BLK_FLOAT_PI / 2.0f, BLK_FLOAT_PI / 2.0f);
        }

        bool DPressed = static_cast<bool>(::GetAsyncKeyState('D'));
        bool APressed = static_cast<bool>(::GetAsyncKeyState('A'));
        int leftRightChange = static_cast<int>(DPressed) - static_cast<int>(APressed);
        if (leftRightChange != 0)
        {
            m_CameraPos += right * (moveDelta * leftRightChange);
        }

        bool WPressed = static_cast<bool>(::GetAsyncKeyState('W'));
        bool SPressed = static_cast<bool>(::GetAsyncKeyState('S'));
        int forwardBackwardChange = static_cast<int>(WPressed) - static_cast<int>(SPressed);
        if (forwardBackwardChange != 0)
        {
            m_CameraPos += forward * (moveDelta * forwardBackwardChange);
        }
    }

    void Camera::UpdateMatrices(float aspectRatio, const Vector3& right, const Vector3& up, const Vector3& forward, Matrix4x4& outViewMatrix, Matrix4x4& outProjMatrix)
    {
        float nearZ = 0.2f;
        float farZ = 1000.0f;

        outProjMatrix = Matrix4x4::CalculateProj(nearZ, farZ, aspectRatio, BLK_DEG_TO_RAD(30.0f));
        outViewMatrix = Matrix4x4::CalculateView(right, up, forward, m_CameraPos);
    }

}