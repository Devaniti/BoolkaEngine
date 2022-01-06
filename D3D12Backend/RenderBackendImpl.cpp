#include "stdafx.h"

#include "RenderBackendImpl.h"

#include "APIWrappers/Resources/ResourceTransition.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"

namespace Boolka
{

    RenderBackendImpl::RenderBackendImpl()
        : m_FrameID(0)
    {
    }

    RenderBackendImpl::~RenderBackendImpl()
    {
        BLK_ASSERT(m_FrameID == 0);
    }

    bool RenderBackendImpl::Initialize(RenderCacheContainer& renderCache)
    {
        BLK_ASSERT(m_FrameID == 0);

        m_FrameID = BLK_IN_FLIGHT_FRAMES;

        bool res = true;
        res = m_Debug.Initialize();
        BLK_CRITICAL_ASSERT(res);
        res = m_Factory.Initialize();
        BLK_CRITICAL_ASSERT(res);
        res = m_Device.Initialize(m_Factory, renderCache);
        BLK_CRITICAL_ASSERT(res);
        res = m_DisplayController.Initialize(m_Device, m_Factory);
        BLK_CRITICAL_ASSERT(res);
        res = m_FrameFence.Initialize(m_Device);
        BLK_CRITICAL_ASSERT(res);
        res = m_RenderSchedule.Initialize(m_Device, m_DisplayController);
        BLK_CRITICAL_ASSERT(res);

        return true;
    }

    void RenderBackendImpl::Unload()
    {
        BLK_ASSERT(m_FrameID != 0);

        m_Device.Flush();

        m_FrameFence.Unload();
        m_RenderSchedule.Unload();
        m_DisplayController.Unload();

        m_Device.Flush();

        m_Device.Unload();
        m_Factory.Unload();
        m_Debug.Unload();

        m_FrameID = 0;
    }

    bool RenderBackendImpl::Present()
    {
        ++m_FrameID;
        return m_DisplayController.Present();
    }

    bool RenderBackendImpl::RenderFrame()
    {
        BLK_ASSERT(m_FrameID >= BLK_IN_FLIGHT_FRAMES);
        // Safe to do without underflow check, since we initialize m_FrameId
        // with BLK_IN_FLIGHT_FRAMES
        m_FrameFence.WaitCPU(m_FrameID - BLK_IN_FLIGHT_FRAMES);

        UINT currentFrameIndex = m_DisplayController.GetCurrentFrameIndex();

        m_RenderSchedule.Render(m_Device, currentFrameIndex);

        m_FrameFence.SignalGPUWithValue(m_FrameID, m_Device.GetGraphicQueue());

        return true;
    }

    bool RenderBackendImpl::LoadScene(SceneData& sceneData)
    {
        bool res = m_RenderSchedule.InitializeResources(m_Device, sceneData);
        BLK_CRITICAL_ASSERT(res);

        return true;
    }

    void RenderBackendImpl::UnloadScene()
    {
        m_Device.Flush();
    }

} // namespace Boolka
