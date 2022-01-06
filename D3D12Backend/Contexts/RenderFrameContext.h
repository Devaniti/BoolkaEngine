#pragma once

#include "APIWrappers/CommandAllocator/GraphicCommandAllocator.h"
#include "Containers/LightContainer.h"
#include "Contexts/FrameStats.h"

namespace Boolka
{

    class Device;
    class RenderEngineContext;

    class [[nodiscard]] RenderFrameContext
    {
    public:
        RenderFrameContext();
        ~RenderFrameContext();

        bool Initialize(Device& device);
        void Unload();

        [[nodiscard]] LightContainer& GetLightContainer();
        [[nodiscard]] const LightContainer& GetLightContainer() const;

        [[nodiscard]] float GetDeltaTime() const;
        [[nodiscard]] UINT GetFrameIndex() const;

        [[nodiscard]] Matrix4x4& GetViewMatrix();
        [[nodiscard]] Matrix4x4& GetProjMatrix();
        [[nodiscard]] Matrix4x4& GetViewProjMatrix();
        [[nodiscard]] Matrix4x4& GetInvViewMatrix();
        [[nodiscard]] Matrix4x4& GetInvProjMatrix();
        [[nodiscard]] Matrix4x4& GetInvViewProjMatrix();
        [[nodiscard]] Vector4* GetEyeRayCoeficients();
        [[nodiscard]] Vector4& GetCameraPos();

        [[nodiscard]] const Matrix4x4& GetViewMatrix() const;
        [[nodiscard]] const Matrix4x4& GetProjMatrix() const;
        [[nodiscard]] const Matrix4x4& GetViewProjMatrix() const;
        [[nodiscard]] const Matrix4x4& GetInvViewMatrix() const;
        [[nodiscard]] const Matrix4x4& GetInvProjMatrix() const;
        [[nodiscard]] const Matrix4x4& GetInvViewProjMatrix() const;
        [[nodiscard]] const Vector4* GetEyeRayCoeficients() const;
        [[nodiscard]] const Vector4& GetCameraPos() const;

#ifdef BLK_ENABLE_STATS
        [[nodiscard]] FrameStats& GetFrameStats() const;
#endif

        void FlipFrame(RenderEngineContext& engineContext, UINT frameIndex);

    private:
        LightContainer m_LightContainer;
        Matrix4x4 m_ViewMatrix;
        Matrix4x4 m_ProjMatrix;
        Matrix4x4 m_ViewProjMatrix;
        Matrix4x4 m_InvViewMatrix;
        Matrix4x4 m_InvProjMatrix;
        Matrix4x4 m_InvViewProjMatrix;
        Vector4 m_EyeRayCoeficients[5];
        Vector4 m_CameraPos;

        float m_DeltaTime;
        UINT m_FrameIndex;

        LARGE_INTEGER m_LastTimestamp;
        LARGE_INTEGER m_Frequency;

#ifdef BLK_ENABLE_STATS
        // Allow to write stats in places which otherwise would get const RenderFrameContext
        mutable FrameStats m_FraneStats;
#endif
    };

} // namespace Boolka
