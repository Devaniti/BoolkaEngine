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

        float GetDeltaTime() { return m_DeltaTime; };
        UINT GetFrameIndex() { return m_FrameIndex; };

        Matrix4x4& GetViewMatrix() { return m_ViewMatrix; };
        Matrix4x4& GetProjMatrix() { return m_ProjMatrix; };

        void FlipFrame(RenderEngineContext& engineContext, UINT frameIndex);
    private:
        Matrix4x4 m_ViewMatrix;
        Matrix4x4 m_ProjMatrix;

        float m_DeltaTime;
        UINT m_FrameIndex;

        LARGE_INTEGER m_LastTimestamp;
        LARGE_INTEGER m_Frequency;
    };

}
