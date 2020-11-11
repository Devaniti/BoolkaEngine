#include "stdafx.h"
#include "DisplayController.h"

#include "APIWrappers/Resources/Textures/Texture2D.h"

namespace Boolka
{

    DisplayController::DisplayController()
        : m_WindowState(WindowState::GetDefault())
    {
    }

    DisplayController::~DisplayController()
    {
    }

    bool DisplayController::Initialize(Device& device, Factory& factory)
    {
        // TODO get window state from save
        WindowState initialWindowState = WindowState::GetDefault();

        bool res = m_Window.Initialize(m_WindowState);
        BLK_CRITICAL_ASSERT(res);
        res = m_Swapchain.Initialize(device, factory, m_Window.GetNativeHandle(), m_WindowState);
        BLK_CRITICAL_ASSERT(res);

        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            res = m_BackBuffers[i].Initialize(m_Swapchain.GetBuffer(i));
            BLK_CRITICAL_ASSERT(res);
        }

        m_Window.ShowWindow(true);

        return true;
    }

    void DisplayController::Unload()
    {
        m_Window.ShowWindow(false);

        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            m_BackBuffers[i].Unload();
        }

        m_Swapchain.Unload();
        m_Window.Unload();
    }

    bool DisplayController::Present()
    {
        return m_Swapchain.Present(m_WindowState);
    }

    bool DisplayController::Update(Device& device)
    {
        throw std::logic_error("Unimplemented");

        // TODO recreate RTVs;

        //if (!m_Window.Update(m_WindowState))
        //    return false;
        //
        //for (auto& backbuffer : m_BackBuffers)
        //{
        //    backbuffer.Unload();
        //}
        //
        //m_Swapchain.Update(device, m_WindowState);
        //
        //for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        //{
        //    bool res = m_BackBuffers[i].Initialize(m_Swapchain.GetBuffer(i));
        //    BLK_CRITICAL_ASSERT(res);
        //}

        return true;
    }

    bool DisplayController::SetWindowState(const WindowState& newWindowState)
    {
        throw std::logic_error("Unimplemented");
    }

    Texture2D& DisplayController::GetBuffer(UINT index)
    {
        return m_BackBuffers[index];
    }

    UINT DisplayController::GetCurrentFrameIndex()
    {
        return m_Swapchain.GetCurrentFrameIndex();
    }

}
