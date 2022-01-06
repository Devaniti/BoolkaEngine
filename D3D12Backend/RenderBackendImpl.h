#pragma once
#include "APIWrappers/Device.h"
#include "APIWrappers/Factory.h"
#include "APIWrappers/RenderDebug.h"
#include "RenderBackend.h"
#include "RenderSchedule/RenderSchedule.h"
#include "WindowManagement/DisplayController.h"

namespace Boolka
{

    class [[nodiscard]] RenderBackendImpl : public RenderBackend
    {
    public:
        RenderBackendImpl();
        ~RenderBackendImpl();

        bool Initialize(RenderCacheContainer& renderCache) final;
        void Unload() final;
        bool Present() final;
        bool RenderFrame() final;
        bool LoadScene(SceneData& sceneData) final;
        void UnloadScene() final;

    private:
        RenderDebug m_Debug;
        Factory m_Factory;
        Device m_Device;
        DisplayController m_DisplayController;
        RenderSchedule m_RenderSchedule;
        Fence m_FrameFence;
        UINT64 m_FrameID;
    };

} // namespace Boolka
