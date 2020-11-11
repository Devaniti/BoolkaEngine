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

    bool Camera::Update(float deltaTime, float aspectRatio, Matrix4x4& outViewMatrix, Matrix4x4& outProjMatrix)
    {
        float speedMult = ::GetAsyncKeyState(VK_SHIFT) ? 5.0f : 1.0f;

        float cameraMoveSpeed = 150.0f * deltaTime * speedMult;
        float cameraRotationSpeed = DEG_TO_RAD(60.0f) * deltaTime;

        static const Vector4 upDirection{ 0, 0, 1, 0 };

        if (::GetAsyncKeyState('R'))
        {
            m_RotationYaw = 0.0f;
            m_RotationPitch = 0.0;
            m_CameraPos = { 0, 0, 100, 0 };
        }

        if (::GetAsyncKeyState(VK_LEFT))
        {
            m_RotationYaw -= cameraRotationSpeed;
            if (m_RotationYaw < DEG_TO_RAD(-180.0f))
                m_RotationYaw += DEG_TO_RAD(360.0f);
        }

        if (::GetAsyncKeyState(VK_RIGHT))
        {
            m_RotationYaw += cameraRotationSpeed;
            if (m_RotationYaw > DEG_TO_RAD(180.0f))
                m_RotationYaw -= DEG_TO_RAD(360.0f);
        }

        if (::GetAsyncKeyState(VK_UP))
        {
            m_RotationPitch += cameraRotationSpeed;
            m_RotationPitch = min(DEG_TO_RAD(90.0f), m_RotationPitch);
        }

        if (::GetAsyncKeyState(VK_DOWN))
        {
            m_RotationPitch -= cameraRotationSpeed;
            m_RotationPitch = max(DEG_TO_RAD(-90.0f), m_RotationPitch);
        }

        Vector4 forward
        {
            cos(m_RotationYaw) * cos(m_RotationPitch),
            sin(m_RotationYaw) * cos(m_RotationPitch),
            sin(m_RotationPitch),
            0.0f
        };

        Vector4 right = upDirection.Cross3(forward).Normalize();
        Vector4 up = forward.Cross3(right).Normalize();


        if (::GetAsyncKeyState('D'))
        {
            m_CameraPos += right * cameraMoveSpeed;
        }

        if (::GetAsyncKeyState('A'))
        {
            m_CameraPos -= right * cameraMoveSpeed;
        }

        if (::GetAsyncKeyState('W'))
        {
            m_CameraPos += forward * cameraMoveSpeed;
        }

        if (::GetAsyncKeyState('S'))
        {
            m_CameraPos -= forward * cameraMoveSpeed;
        }

        float nearZ = 1.0f;
        float farZ = 40000.0f;

        //float viewHeight = 300;

        //float w = 2.0f / viewHeight / aspectRatio;
        //float h = 2.0f / viewHeight;
        //float a = 1.0f / (farZ - nearZ);
        //float b = -a * nearZ;

        //static const Matrix4x4 ortProj =
        //{
        //    w, 0, 0, 0,
        //    0, h, 0, 0,
        //    0, 0, a, 0,
        //    0, 0, b, 1,
        //};

        // Perspective projection
        float fovY = DEG_TO_RAD(30.0f);

        float h = 1.0f / tan(fovY * 0.5f);
        float w = h / aspectRatio;
        float a = farZ / (farZ - nearZ);
        float b = (-nearZ * farZ) / (farZ - nearZ);

        outProjMatrix =
        {
            w, 0, 0, 0,
            0, h, 0, 0,
            0, 0, a, 1,
            0, 0, b, 0,
        };

        outViewMatrix = Matrix4x4::GetView(
            right,
            up,
            forward,
            m_CameraPos
        );

        return true;
    }

}