#include "stdafx.h"

#include "RenderFrameContext.h"

#include "BoolkaCommon/DebugHelpers/DebugOutputStream.h"
#include "Contexts/RenderEngineContext.h"

namespace Boolka
{

    RenderFrameContext::RenderFrameContext()
        : m_DeltaTime(0.0f)
        , m_FrameIndex(0)
        , m_LastTimestamp{}
        , m_Frequency()
#ifdef BLK_ENABLE_STATS
        , m_FraneStats()
#endif
    {
    }

    RenderFrameContext::~RenderFrameContext()
    {
        BLK_ASSERT(m_DeltaTime == 0.0f);
        BLK_ASSERT(m_FrameIndex == 0);
        BLK_ASSERT(m_LastTimestamp.QuadPart == 0);
        BLK_ASSERT(m_Frequency.QuadPart == 0);
    }

    bool RenderFrameContext::Initialize(Device& device)
    {
        BOOL res = ::QueryPerformanceFrequency(&m_Frequency);
        BLK_CRITICAL_ASSERT(res);
        res = ::QueryPerformanceCounter(&m_LastTimestamp);
        BLK_CRITICAL_ASSERT(res);

        return true;
    }

    void RenderFrameContext::Unload()
    {
        m_DeltaTime = 0.0f;
        m_FrameIndex = 0;
        m_LastTimestamp = {};
        m_Frequency = {};
    }

    const Matrix4x4& RenderFrameContext::GetProjMatrix() const
    {
        return m_ProjMatrix;
    }

    Matrix4x4& RenderFrameContext::GetProjMatrix()
    {
        return m_ProjMatrix;
    }

    const Matrix4x4& RenderFrameContext::GetViewProjMatrix() const
    {
        return m_ViewProjMatrix;
    }

    Matrix4x4& RenderFrameContext::GetViewProjMatrix()
    {
        return m_ViewProjMatrix;
    }

    const Matrix4x4& RenderFrameContext::GetInvViewMatrix() const
    {
        return m_InvViewMatrix;
    }

    Matrix4x4& RenderFrameContext::GetInvViewMatrix()
    {
        return m_InvViewMatrix;
    }

    const Matrix4x4& RenderFrameContext::GetInvProjMatrix() const
    {
        return m_InvProjMatrix;
    }

    Matrix4x4& RenderFrameContext::GetInvProjMatrix()
    {
        return m_InvProjMatrix;
    }

    const Matrix4x4& RenderFrameContext::GetInvViewProjMatrix() const
    {
        return m_InvViewProjMatrix;
    }

    Matrix4x4& RenderFrameContext::GetInvViewProjMatrix()
    {
        return m_InvViewProjMatrix;
    }

    const Vector4& RenderFrameContext::GetCameraPos() const
    {
        return m_CameraPos;
    }

    Vector4& RenderFrameContext::GetCameraPos()
    {
        return m_CameraPos;
    }

    const LightContainer& RenderFrameContext::GetLightContainer() const
    {
        return m_LightContainer;
    }

    LightContainer& RenderFrameContext::GetLightContainer()
    {
        return m_LightContainer;
    }

    float RenderFrameContext::GetDeltaTime() const
    {
        return m_DeltaTime;
    }

    UINT RenderFrameContext::GetFrameIndex() const
    {
        return m_FrameIndex;
    }

    const Matrix4x4& RenderFrameContext::GetViewMatrix() const
    {
        return m_ViewMatrix;
    }

    Matrix4x4& RenderFrameContext::GetViewMatrix()
    {
        return m_ViewMatrix;
    }

    FrameStats& RenderFrameContext::GetFrameStats() const
    {
        return m_FraneStats;
    }

    void RenderFrameContext::FlipFrame(RenderEngineContext& engineContext, UINT frameIndex)
    {
        m_FrameIndex = frameIndex;

        LARGE_INTEGER currentTimestamp;
        BOOL res = ::QueryPerformanceCounter(&currentTimestamp);
        BLK_ASSERT_VAR(res);

        LARGE_INTEGER timestampDifference = currentTimestamp - m_LastTimestamp;

        m_LastTimestamp = currentTimestamp;

        m_DeltaTime = timestampDifference / m_Frequency;

#ifdef BLK_ENABLE_STATS
        m_FraneStats.frameTime = m_DeltaTime;
        m_FraneStats.frameTimeStable = 0.05f * m_DeltaTime + 0.95f * m_FraneStats.frameTimeStable;
#endif

        // Clamp max delta time to 30 fps equivalent
        m_DeltaTime = min(0.033f, m_DeltaTime);

        UINT width = engineContext.GetBackbufferWidth();
        UINT height = engineContext.GetBackbufferHeight();

        float aspectRatio = static_cast<float>(width) / height;

        engineContext.GetCamera().Update(m_DeltaTime, aspectRatio, m_ViewMatrix, m_ProjMatrix,
                                         m_CameraPos);

        m_ViewProjMatrix = m_ViewMatrix * m_ProjMatrix;
        bool isSuccessfull;
        m_InvViewMatrix = m_ViewMatrix.Inverse(isSuccessfull);
        BLK_ASSERT(isSuccessfull);
        m_InvProjMatrix = m_ProjMatrix.Inverse(isSuccessfull);
        BLK_ASSERT(isSuccessfull);
        m_InvViewProjMatrix = m_ViewProjMatrix.Inverse(isSuccessfull);
        BLK_ASSERT(isSuccessfull);

        m_LightContainer.Update(m_DeltaTime);
    }

} // namespace Boolka
