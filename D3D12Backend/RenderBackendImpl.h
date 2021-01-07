#pragma once
#include "RenderBackend.h"

#include "APIWrappers/Factory.h"
#include "APIWrappers/RenderDebug.h"
#include "APIWrappers/Device.h"
#include "WindowManagement/DisplayController.h"
#include "RenderSchedule/RenderSchedule.h"

namespace Boolka
{

	class RenderBackendImpl : public RenderBackend
	{
	public:
		RenderBackendImpl();
		~RenderBackendImpl();

		bool Initialize() final;
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

}
