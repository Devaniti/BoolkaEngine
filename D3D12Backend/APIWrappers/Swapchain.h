#pragma once

namespace Boolka
{

    class Device;
    class Factory;
    struct WindowState;

    class Swapchain
    {
    public:
        Swapchain();
        ~Swapchain();

        IDXGISwapChain4* Get() { BLK_ASSERT(m_Swapchain != nullptr); return m_Swapchain; };
        IDXGISwapChain4* operator->() { return Get(); };

        bool Present(const WindowState& windowState);
        ID3D12Resource* GetBuffer(UINT index);
        UINT GetCurrentFrameIndex();

        bool Initialize(Device& device, Factory& factory, HWND window, WindowState& windowState);
        void Unload();

        bool Update(Device& device, WindowState windowState);
    private:
        IDXGISwapChain4* m_Swapchain;
        bool m_IsFullscreen;
    };

}
