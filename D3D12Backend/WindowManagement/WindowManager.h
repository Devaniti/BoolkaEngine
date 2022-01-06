#pragma once
#include "WindowState.h"

namespace Boolka
{

    class [[nodiscard]] WindowManager
    {
    public:
        WindowManager();
        ~WindowManager();

        bool Initialize(const WindowState& windowState);
        void Unload();

        bool Update(WindowState& stateToUpdate);

        [[nodiscard]] HWND GetHWND() const;

        void ShowWindow(bool show);

    private:
        // WindowThread only
        void WindowThreadEntryPoint(WindowState windowState);
        void InitializeThread();
        void InitializeWindow(const WindowState& windowState);
        void MessageLoop();
        [[nodiscard]] static LRESULT CALLBACK WindowProcDetour(HWND hwnd, UINT message,
                                                               WPARAM wParam, LPARAM lParam);
        [[nodiscard]] LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                                  LPARAM lParam);

        // Helper functions
        [[nodiscard]] static DWORD CalculateWindowStyle(WindowState::WindowMode windowMode);

        HWND m_HWND;
        std::thread m_WindowThread;
        std::atomic<bool> m_IsUpdated;

        static HMODULE ms_CurrentInstance;
        static ATOM ms_WindowClass;
    };

} // namespace Boolka
