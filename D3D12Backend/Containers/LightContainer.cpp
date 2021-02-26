#include "stdafx.h"

#include "LightContainer.h"

namespace Boolka
{

    LightContainer::LightContainer() 
        : m_Lights(BLK_MAX_LIGHT_COUNT)
        , m_ViewProjMatrices(BLK_MAX_LIGHT_COUNT)
        , m_CurrentRotation(0.0f)
    {
    }

    void LightContainer::Update(float deltaTime)
    {
        m_CurrentRotation += deltaTime * 0.25f;

        static const float nearZ = 0.1f;
        static const float farZ = 10.0f;

        Vector3 center = { 14.0f, 6.5f, 2.0f };
        float distance = 6.0f;
        Vector4 colors[BLK_MAX_LIGHT_COUNT] =
        {
            {4.0f, 4.0f, 4.0f, 0.0f},
            {4.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 4.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 4.0f, 0.0f},
        };

        Matrix4x4 proj = Matrix4x4::CalculateProj(nearZ, farZ, 1.0f, BLK_DEG_TO_RAD(90.0f));

        for (size_t i = 0; i < BLK_MAX_LIGHT_COUNT; ++i)
        {
            float rotation = m_CurrentRotation + BLK_FLOAT_PI / 2.0f * i;
            Vector3 fromCenter = { distance * sin(rotation), distance * cos(rotation), 0.0f };
            Vector3 worldPos = Vector4(center + fromCenter);
            m_Lights[i].worldPos = worldPos;
            m_Lights[i].color = colors[i];
            m_Lights[i].nearZ = nearZ;
            m_Lights[i].farZ = farZ;

            for (size_t j = 0; j < BLK_TEXCUBE_FACE_COUNT; ++j)
            {
                m_ViewProjMatrices[i][j] = Matrix4x4::CalculateCubeMapView(j, worldPos) * proj;
            }
        }
    }

}
