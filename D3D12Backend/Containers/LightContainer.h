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

        struct Sun
        {
            Vector3 lightDir;
            Vector3 worldPos;
            Vector3 color;
        };

        const std::vector<Light>& GetLights() const { return m_Lights; }
        const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& GetViewProjMatrices() const { return m_ViewProjMatrices; }

        const Sun& GetSun() const { return m_Sun; }
        const Matrix4x4& GetSunView() const { return m_SunView; }
        const Matrix4x4& GetSunProj() const { return m_SunProj; }

        void Update(float deltaTime);

    private:
        void UpdateLights();
        void UpdateSun();

        std::vector<Light> m_Lights;
        std::vector<std::array<Matrix4x4, 6>> m_ViewProjMatrices;
        Sun m_Sun;
        Matrix4x4 m_SunView;
        Matrix4x4 m_SunProj;
        float m_CurrentRotation;
    };

}
