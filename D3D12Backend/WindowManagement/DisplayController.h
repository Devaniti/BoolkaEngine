#pragma once
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Swapchain.h"
#include "WindowManager.h"

namespace Boolka
{

    class Factory;
    class Device;

    class DisplayController
    {
    public:
        DisplayController();
        ~DisplayController();

        bool Initialize(Device& device, Factory& factory);
        void Unload();

        bool Present();

        // returns true if something was updated, or false otherwise
        bool Update(Device& device);
        bool SetWindowState(const WindowState& newWindowState);

        Texture2D& GetBuffer(UINT index);
        UINT GetCurrentFrameIndex();

        const WindowState& GetWindowState();
        HWND GetHWND() const;

    private:
        WindowManager m_Window;
        Swapchain m_Swapchain;
        Texture2D m_BackBuffers[BLK_IN_FLIGHT_FRAMES];
        WindowState m_WindowState;
    };

} // namespace Boolka
