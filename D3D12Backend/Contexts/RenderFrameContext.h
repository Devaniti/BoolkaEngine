#pragma once

#include "APIWrappers/CommandAllocator/GraphicCommandAllocator.h"
#include "Contexts/FrameStats.h"
#include "Containers/LightContainer.h"

namespace Boolka
{

    class Device;
    class RenderEngineContext;

    class RenderFrameContext
    {
    public:
        RenderFrameContext();
        ~RenderFrameContext();

        bool Initialize(Device& device);
        void Unload();

        LightContainer& GetLightContainer() { return m_LightContainer; }

        float GetDeltaTime() const { return m_DeltaTime; };
        UINT GetFrameIndex() const { return m_FrameIndex; };

        Matrix4x4& GetViewMatrix() { return m_ViewMatrix; };
        Matrix4x4& GetProjMatrix() { return m_ProjMatrix; };
        Matrix4x4& GetViewProjMatrix() { return m_ViewProjMatrix; };
        Matrix4x4& GetInvViewMatrix() { return m_InvViewMatrix; };
        Matrix4x4& GetInvProjMatrix() { return m_InvProjMatrix; };
        Matrix4x4& GetInvViewProjMatrix() { return m_InvViewProjMatrix; };
        Vector3& GetCameraPos() { return m_CameraPos; };

        const Matrix4x4& GetViewMatrix() const { return m_ViewMatrix; };
        const Matrix4x4& GetProjMatrix() const { return m_ProjMatrix; };
        const Matrix4x4& GetViewProjMatrix() const { return m_ViewProjMatrix; };
        const Matrix4x4& GetInvViewMatrix() const { return m_InvViewMatrix; };
        const Matrix4x4& GetInvProjMatrix() const { return m_InvProjMatrix; };
        const Matrix4x4& GetInvViewProjMatrix() const { return m_InvViewProjMatrix; };
        const Vector3& GetCameraPos() const { return m_CameraPos; };

#ifdef BLK_ENABLE_STATS
        FrameStats& GetFrameStats() { return m_FraneStats; };
        const FrameStats& GetFrameStats() const { return m_FraneStats; };
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
        Vector3 m_CameraPos;

        float m_DeltaTime;
        UINT m_FrameIndex;

        LARGE_INTEGER m_LastTimestamp;
        LARGE_INTEGER m_Frequency;

#ifdef BLK_ENABLE_STATS
        FrameStats m_FraneStats;
#endif
    };

}
