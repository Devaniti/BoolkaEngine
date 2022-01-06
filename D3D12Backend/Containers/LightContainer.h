#pragma once

namespace Boolka
{
    // Temp container for lights
    class [[nodiscard]] LightContainer
    {
    public:
        LightContainer();

        struct [[nodiscard]] Light
        {
            Vector3 worldPos;
            float nearZ;
            Vector3 color;
            float farZ;
        };

        struct [[nodiscard]] Sun
        {
            Vector4 lightDir;
            Vector4 worldPos;
            Vector4 color;
        };

        [[nodiscard]] const std::vector<Light>& GetLights() const;
        [[nodiscard]] const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>&
        GetViewProjMatrices() const;
        [[nodiscard]] const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>&
        GetViewMatrices() const;
        [[nodiscard]] const std::vector<std::array<Matrix4x4, BLK_TEXCUBE_FACE_COUNT>>&
        GetProjMatrices() const;

        [[nodiscard]] const Sun& GetSun() const;
        [[nodiscard]] const Matrix4x4& GetSunView() const;
        [[nodiscard]] const Matrix4x4& GetSunProj() const;
        [[nodiscard]] const Matrix4x4& GetSunViewProj() const;

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
        Matrix4x4 m_SunViewProj;
        float m_CurrentRotation;
    };

} // namespace Boolka
