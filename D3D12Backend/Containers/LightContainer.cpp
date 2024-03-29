#include "stdafx.h"

#include "LightContainer.h"

namespace Boolka
{

    LightContainer::LightContainer()
        : m_Lights(BLK_MAX_LIGHT_COUNT)
        , m_ViewProjMatrices(BLK_MAX_LIGHT_COUNT)
        , m_ViewMatrices(BLK_MAX_LIGHT_COUNT)
        , m_ProjMatrices(BLK_MAX_LIGHT_COUNT)
        , m_CurrentRotation(0.0f)
    {
    }

    const std::vector<LightContainer::Light>& LightContainer::GetLights() const
    {
        return m_Lights;
    }

    const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& LightContainer::
        GetViewProjMatrices() const
    {
        return m_ViewProjMatrices;
    }

    const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& LightContainer::
        GetViewMatrices() const
    {
        return m_ViewMatrices;
    }

    const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& LightContainer::
        GetProjMatrices() const
    {
        return m_ProjMatrices;
    }

    const LightContainer::Sun& LightContainer::GetSun() const
    {
        return m_Sun;
    }

    const Matrix4x4& LightContainer::GetSunView() const
    {
        return m_SunView;
    }

    const Matrix4x4& LightContainer::GetSunProj() const
    {
        return m_SunProj;
    }

    const Matrix4x4& LightContainer::GetSunViewProj() const
    {
        return m_SunViewProj;
    }

    void LightContainer::Update(float deltaTime)
    {
        m_CurrentRotation += deltaTime * 0.25f;
        UpdateLights();
        UpdateSun();
    }

    void LightContainer::UpdateLights()
    {
        static const float nearZ = 0.1f;
        static const float farZ = 7.0f;

        static const float intensity = 9.0f;

        Vector3 center = {14.0f, 6.5f, 2.0f};
        float distance = 6.0f;
        Vector4 colors[BLK_MAX_LIGHT_COUNT] = {
            Vector4{1.0f, 1.0f, 1.0f, 0.0f} * intensity,
            Vector4{1.0f, 0.7f, 0.7f, 0.0f} * intensity,
            Vector4{0.7f, 1.0f, 0.7f, 0.0f} * intensity,
            Vector4{0.7f, 0.7f, 1.0f, 0.0f} * intensity,
        };

        Matrix4x4 proj =
            Matrix4x4::CalculateProjPerspective(nearZ, farZ, 1.0f, BLK_DEG_TO_RAD(90.0f));

        for (size_t i = 0; i < m_Lights.size(); ++i)
        {
            float rotation = m_CurrentRotation + BLK_FLOAT_PI / 2.0f * i;
            Vector3 fromCenter = {distance * sin(rotation), distance * cos(rotation), 0.0f};
            Vector3 worldPos = center + fromCenter;
            m_Lights[i].worldPos = worldPos;
            m_Lights[i].color = colors[i];
            m_Lights[i].nearZ = nearZ;
            m_Lights[i].farZ = farZ;

            for (size_t j = 0; j < BLK_TEXCUBE_FACE_COUNT; ++j)
            {
                m_ViewMatrices[i][j] = Matrix4x4::CalculateCubeMapView(j, worldPos);
                m_ProjMatrices[i][j] = proj;
                m_ViewProjMatrices[i][j] = m_ViewMatrices[i][j] * proj;

                Frustum testFrustum = Frustum(m_ViewProjMatrices[i][j]);

                static const Vector3 forward[6] = {
                    {1.0f, 0.0f, 0.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                    {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, -1.0f},
                };

                Vector4 testPoint = Vector4(worldPos + forward[j], 1.0f);

                bool test = testFrustum.CheckPoint(testPoint);
                BLK_ASSERT(test);
            }
        }
    }

    void LightContainer::UpdateSun()
    {
        static const float nearZ = 0.1f;
        static const float farZ = 40.0f;
        static const float worldWidth = 100.0f;
        static const Vector4 color{0.45f, 0.45f, 0.25f};
        static const Vector4 lightDir = Vector4{-0.1f, -0.4f, -1.0f}.Normalize();
        static const Vector4 worldPos =
            Vector4(15.0f, 0.0f, 0.0f, 1.0f) - lightDir * (farZ - nearZ) / 2.0f;
        static const Vector4 upDirection{0, 0, 1};
        static const Vector4 right = upDirection.Cross(lightDir).Normalize();
        static const Vector4 up = lightDir.Cross(right).Normalize();
        static const Matrix4x4 view = Matrix4x4::CalculateView(right, up, lightDir, worldPos);
        static const Matrix4x4 proj =
            Matrix4x4::CalculateProjOrtographic(nearZ, farZ, worldWidth, worldWidth);

        m_Sun.lightDir = lightDir;
        m_Sun.worldPos = worldPos;
        m_Sun.color = color;

        m_SunView = view;
        m_SunProj = proj;
        m_SunViewProj = view * proj;
    }

} // namespace Boolka
