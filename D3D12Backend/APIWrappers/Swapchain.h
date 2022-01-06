#pragma once

namespace Boolka
{

    class Device;
    class Factory;
    struct WindowState;

    class [[nodiscard]] Swapchain
    {
    public:
        Swapchain();
        ~Swapchain();

        [[nodiscard]] IDXGISwapChain4* Get();
        [[nodiscard]] IDXGISwapChain4* operator->();

        // Present CAN fail, have to check return code
        [[nodiscard]] bool Present(const WindowState& windowState);
        [[nodiscard]] ID3D12Resource* GetBuffer(UINT index);
        [[nodiscard]] UINT GetCurrentFrameIndex();

        bool Initialize(Device& device, Factory& factory, HWND window, WindowState& windowState);
        void Unload();

        bool Update(Device& device, WindowState windowState);

    private:
        IDXGISwapChain4* m_Swapchain;
        bool m_IsFullscreen;
        UINT m_PresentFlags;
    };

} // namespace Boolka
