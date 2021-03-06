#include "stdafx.h"

#include "WindowManager.h"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_helper.h"

// Private window messages
#define WM_CUSTOM_FORCE_CLOSE (WM_APP + 1)
#define WM_CUSTOM_SET_FOCUS (WM_CUSTOM_FORCE_CLOSE + 1)

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

namespace Boolka
{
    HMODULE WindowManager::ms_CurrentInstance = NULL;
    ATOM WindowManager::ms_WindowClass = 0;

    WindowManager::WindowManager()
        : m_HWND(0)
        , m_WindowThread()
        , m_IsUpdated(false)
    {
    }

    WindowManager::~WindowManager()
    {
        BLK_ASSERT(m_HWND == NULL);
    }

    bool WindowManager::Initialize(const WindowState& windowState)
    {
        BLK_ASSERT(m_HWND == 0);

        if (ms_CurrentInstance == 0)
        {
            ms_CurrentInstance = ::GetModuleHandle(nullptr);
            BLK_CRITICAL_ASSERT(ms_CurrentInstance != 0);
        }

        if (ms_WindowClass == 0)
        {
            WNDCLASSEXW windowClassDesc = {};
            windowClassDesc.cbSize = sizeof(windowClassDesc);
            windowClassDesc.style = CS_HREDRAW | CS_VREDRAW;
            windowClassDesc.lpfnWndProc = &WindowProcDetour;
            windowClassDesc.hInstance = ms_CurrentInstance;
            windowClassDesc.lpszClassName = BLK_WINDOW_CLASS_NAME;

            ms_WindowClass = RegisterClassExW(&windowClassDesc);
            BLK_CRITICAL_ASSERT(ms_WindowClass != 0);
        }

        m_WindowThread = std::thread(&WindowManager::WindowThreadEntryPoint, this, windowState);

        // TODO better wait
        while (!m_IsUpdated)
        {
        };

        return m_HWND != NULL;
    }

    void WindowManager::Unload()
    {
        BLK_ASSERT(m_HWND != 0);
        BOOL res = ::PostMessage(m_HWND, WM_CUSTOM_FORCE_CLOSE, NULL, NULL);
        BLK_ASSERT(res != 0);
        m_WindowThread.join();
    }

    bool WindowManager::Update(WindowState& stateToUpdate)
    {
        if (m_IsUpdated)
            return false;

        // User can only update width/height
        RECT windowClientRect = {};
        BOOL res = ::GetClientRect(m_HWND, &windowClientRect);
        BLK_ASSERT(res);

        stateToUpdate.width = windowClientRect.right - windowClientRect.left;
        stateToUpdate.height = windowClientRect.bottom - windowClientRect.top;

        return true;
    }

    HWND WindowManager::GetHWND() const
    {
        return m_HWND;
    }

    void WindowManager::ShowWindow(bool show)
    {
        BLK_ASSERT(m_HWND != 0);
        ::ShowWindow(m_HWND, show ? SW_SHOW : SW_HIDE);
        if (show)
            ::PostMessage(m_HWND, WM_CUSTOM_SET_FOCUS, NULL, NULL);
    }

    LRESULT CALLBACK WindowManager::WindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                               LPARAM lParam)
    {
        {
            const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);
            if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
                return true;
        }

        switch (message)
        {
        case WM_CUSTOM_FORCE_CLOSE:
            ::DestroyWindow(hwnd);
            return 0;
        case WM_CUSTOM_SET_FOCUS: {
            HWND res = ::SetFocus(hwnd);
            BLK_ASSERT(res != 0);
        }
            return 0;
        case WM_CLOSE:
            BLK_NOOP();
            return 0;
        case WM_SIZE:
            m_IsUpdated = false;
            break;
        }

        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    DWORD WindowManager::CalculateWindowStyle(WindowState::WindowMode windowMode)
    {
        switch (windowMode)
        {
        case WindowState::WindowMode::Windowed:
            return WS_OVERLAPPEDWINDOW;
            break;
        case WindowState::WindowMode::Borderless:
        case WindowState::WindowMode::Fullscreen:
            return WS_POPUP;
            break;
        default:
            BLK_ASSERT(0);
            break;
        }

        return WS_OVERLAPPEDWINDOW;
    }

    void WindowManager::WindowThreadEntryPoint(WindowState windowState)
    {
        InitializeThread();
        InitializeWindow(windowState);
        m_IsUpdated = true;
        MessageLoop();
        m_IsUpdated = false;
        m_HWND = NULL;
    }

    void WindowManager::InitializeThread()
    {
        BLK_SET_CURRENT_THREAD_NAME(L"Window Thread");
        ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    }

    void WindowManager::InitializeWindow(const WindowState& windowState)
    {
        RECT calculatedRect;
        calculatedRect.left = 0;
        calculatedRect.top = 0;
        calculatedRect.right = windowState.width;
        calculatedRect.bottom = windowState.height;

        DWORD windowStyle = CalculateWindowStyle(windowState.windowMode);

        ::AdjustWindowRect(&calculatedRect, windowStyle, FALSE);

        m_HWND = CreateWindowExW(
            NULL, BLK_WINDOW_CLASS_NAME, BLK_GAME_NAME, windowStyle, windowState.x, windowState.y,
            calculatedRect.right - calculatedRect.left, calculatedRect.bottom - calculatedRect.top,
            NULL, NULL, ms_CurrentInstance, this);

        BLK_CRITICAL_ASSERT(m_HWND != NULL);
    }

    void WindowManager::MessageLoop()
    {
        MSG msg;
        BOOL bRet;

        while ((bRet = ::GetMessage(&msg, m_HWND, 0, 0)) != 0)
        {
            if (bRet == -1)
            {
                return;
            }
            else
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
    }

    LRESULT CALLBACK WindowManager::WindowProcDetour(HWND hwnd, UINT message, WPARAM wParam,
                                                     LPARAM lParam)
    {
        // How can I make a WNDPROC or DLGPROC a member of my C++ class?
        // https://devblogs.microsoft.com/oldnewthing/20140203-00/?p=1893
        WindowManager* object;
        if (message == WM_NCCREATE)
        {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            object = static_cast<WindowManager*>(lpcs->lpCreateParams);
            ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(object));
        }
        else
        {
            object = reinterpret_cast<WindowManager*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }
        if (object)
        {
            return object->WindowProc(hwnd, message, wParam, lParam);
        }

        return ::DefWindowProc(hwnd, message, wParam, lParam);
    }

} // namespace Boolka
