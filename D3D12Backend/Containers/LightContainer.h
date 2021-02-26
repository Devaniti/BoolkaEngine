#pragma once

namespace Boolka
{
    // Temp container for lights
    class LightContainer
    {
    public:

        LightContainer();

        struct Light
        {
            Vector3 worldPos;
            float nearZ;
            Vector3 color;
            float farZ;
        };

        std::vector<Light>& GetLights() { return m_Lights; }
        std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& GetViewProjMatrices() { return m_ViewProjMatrices; }

        const std::vector<Light>& GetLights() const { return m_Lights; }
        const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& GetViewProjMatrices() const { return m_ViewProjMatrices; }

        void Update(float deltaTime);

    private:
        std::vector<Light> m_Lights;
        std::vector<std::array<Matrix4x4, 6>> m_ViewProjMatrices;
        float m_CurrentRotation;
    };

}
