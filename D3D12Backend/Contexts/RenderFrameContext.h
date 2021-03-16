#pragma once

#include "APIWrappers/CommandAllocator/GraphicCommandAllocator.h"
#include "Containers/LightContainer.h"
#include "Contexts/FrameStats.h"

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

        LightContainer& GetLightContainer();
        const LightContainer& GetLightContainer() const;

        float GetDeltaTime() const;
        UINT GetFrameIndex() const;

        Matrix4x4& GetViewMatrix();
        Matrix4x4& GetProjMatrix();
        Matrix4x4& GetViewProjMatrix();
        Matrix4x4& GetInvViewMatrix();
        Matrix4x4& GetInvProjMatrix();
        Matrix4x4& GetInvViewProjMatrix();
        Vector4& GetCameraPos();

        const Matrix4x4& GetViewMatrix() const;
        const Matrix4x4& GetProjMatrix() const;
        const Matrix4x4& GetViewProjMatrix() const;
        const Matrix4x4& GetInvViewMatrix() const;
        const Matrix4x4& GetInvProjMatrix() const;
        const Matrix4x4& GetInvViewProjMatrix() const;
        const Vector4& GetCameraPos() const;

#ifdef BLK_ENABLE_STATS
        FrameStats& GetFrameStats() const;
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
