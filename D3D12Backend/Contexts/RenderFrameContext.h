#pragma once

#include "APIWrappers/CommandAllocator/GraphicCommandAllocator.h"

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

        float GetDeltaTime() const { return m_DeltaTime; };
        UINT GetFrameIndex() const { return m_FrameIndex; };

        Matrix4x4& GetViewMatrix() { return m_ViewMatrix; };
        Matrix4x4& GetProjMatrix() { return m_ProjMatrix; };
        Vector4& GetCameraPos() { return m_CameraPos; };

        const Matrix4x4& GetViewMatrix() const { return m_ViewMatrix; };
        const Matrix4x4& GetProjMatrix() const { return m_ProjMatrix; };
        const Vector4& GetCameraPos() const { return m_CameraPos; };

        void FlipFrame(RenderEngineContext& engineContext, UINT frameIndex);
    private:
        Matrix4x4 m_ViewMatrix;
        Matrix4x4 m_ProjMatrix;
        Vector4 m_CameraPos;

        float m_DeltaTime;
        UINT m_FrameIndex;

        LARGE_INTEGER m_LastTimestamp;
        LARGE_INTEGER m_Frequency;
    };

}
