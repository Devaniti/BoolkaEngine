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
            Vector4 lightDir;
            Vector4 worldPos;
            Vector4 color;
        };

        const std::vector<Light>& GetLights() const;
        const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& GetViewProjMatrices()
            const;
        const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& GetViewMatrices() const;
        const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>& GetProjMatrices() const;

        const Sun& GetSun() const;
        const Matrix4x4& GetSunView() const;
        const Matrix4x4& GetSunProj() const;

        void Update(float deltaTime);

    private:
        void UpdateLights();
        void UpdateSun();

        std::vector<Light> m_Lights;
        std::vector<std::array<Matrix4x4, 6>> m_ViewProjMatrices;
        std::vector<std::array<Matrix4x4, 6>> m_ViewMatrices;
        std::vector<std::array<Matrix4x4, 6>> m_ProjMatrices;
        Sun m_Sun;
        Matrix4x4 m_SunView;
        Matrix4x4 m_SunProj;
        float m_CurrentRotation;
    };

} // namespace Boolka
