#pragma once
#include "WindowState.h"

namespace Boolka
{

    class WindowManager
    {
    public:
        WindowManager();
        ~WindowManager();

        bool Initialize(const WindowState& windowState);
        void Unload();

        bool Update(WindowState& stateToUpdate);

        HWND GetNativeHandle() { return m_Window; };

        void ShowWindow(bool show);
    private:

        // WindowThread only
        void WindowThreadEntryPoint(WindowState windowState);
        void InitializeThread();
        void InitializeWindow(const WindowState& windowState);
        void MessageLoop();
        static LRESULT CALLBACK WindowProcDetour(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        // Helper functions
        static DWORD CalculateWindowStyle(WindowState::WindowMode windowMode);

        HWND m_Window;
        std::thread m_WindowThread;
        std::atomic<bool> m_IsUpdated;

        static HMODULE ms_CurrentInstance;
        static ATOM ms_WindowClass;
    };

}
