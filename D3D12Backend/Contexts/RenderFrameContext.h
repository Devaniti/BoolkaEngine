#pragma once

#include "APIWrappers/CommandAllocator/GraphicCommandAllocator.h"

namespace Boolka
{

    class Device;

    class RenderFrameContext
    {
    public:
        RenderFrameContext();
        ~RenderFrameContext();

        bool Initialize(Device& device);
        void Unload();

        float GetDeltaTime() { return m_DeltaTime; };
        UINT GetFrameIndex() { return m_FrameIndex; };

        void FlipFrame(UINT frameIndex);
    private:
        float m_DeltaTime;
        UINT m_FrameIndex;

        LARGE_INTEGER m_LastTimestamp;
        LARGE_INTEGER m_Frequency;
    };

}
