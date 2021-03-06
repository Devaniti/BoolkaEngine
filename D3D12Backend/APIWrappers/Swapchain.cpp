#include "stdafx.h"

#include "Swapchain.h"

#include "Device.h"
#include "Factory.h"
#include "WindowManagement/WindowState.h"

namespace Boolka
{

    Swapchain::Swapchain()
        : m_Swapchain(nullptr)
        , m_IsFullscreen(false)
        , m_PresentFlags(0)
    {
    }

    Swapchain::~Swapchain()
    {
        BLK_ASSERT(m_Swapchain == nullptr);
        BLK_ASSERT(m_IsFullscreen == false);
        BLK_ASSERT(m_PresentFlags == 0);
    }

    IDXGISwapChain4* Swapchain::Get()
    {
        BLK_ASSERT(m_Swapchain != nullptr);
        return m_Swapchain;
    }

    IDXGISwapChain4* Swapchain::operator->()
    {
        return Get();
    }

    bool Swapchain::Initialize(Device& device, Factory& factory, HWND window,
                               WindowState& windowState)
    {
        static_assert(BLK_IN_FLIGHT_FRAMES > 1,
                      "DXGI_SWAP_EFFECT_FLIP_DISCARD swapchain only support 2+ "
                      "backbuffers");

        BLK_ASSERT(m_Swapchain == nullptr);
        BLK_ASSERT(m_IsFullscreen == false);
        BLK_ASSERT(m_PresentFlags == 0);

        ID3D12CommandQueue* graphicQueue = device.GetGraphicQueue().Get();
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = 0;
        desc.Height = 0;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Stereo = FALSE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = BLK_IN_FLIGHT_FRAMES;
        desc.Scaling = DXGI_SCALING_NONE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.Flags =
            DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        IDXGISwapChain1* swapchain1 = nullptr;
        HRESULT hr = factory->CreateSwapChainForHwnd(graphicQueue, window, &desc, nullptr, nullptr,
                                                     &swapchain1);
        BLK_CRITICAL_ASSERT(SUCCEEDED(hr));

        hr = factory->MakeWindowAssociation(window,
                                            DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN);
        BLK_CRITICAL_ASSERT(SUCCEEDED(hr));

        hr = swapchain1->QueryInterface(&m_Swapchain);
        swapchain1->Release();
        BLK_CRITICAL_ASSERT(SUCCEEDED(hr));

        hr = m_Swapchain->SetMaximumFrameLatency(BLK_IN_FLIGHT_FRAMES - 1);
        BLK_CRITICAL_ASSERT(SUCCEEDED(hr));
        if (windowState.windowMode == WindowState::WindowMode::Fullscreen)
        {
            m_Swapchain->SetFullscreenState(TRUE, NULL);
            m_Swapchain->ResizeBuffers(BLK_IN_FLIGHT_FRAMES, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM,
                                       DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            m_IsFullscreen = true;
        }

        m_PresentFlags = 0;
        if (windowState.presentInterval == 0 &&
            windowState.windowMode != WindowState::WindowMode::Fullscreen)
            m_PresentFlags |= DXGI_PRESENT_ALLOW_TEARING;

        return SUCCEEDED(hr);
    }

    void Swapchain::Unload()
    {
        BLK_ASSERT(m_Swapchain != nullptr);

        if (m_IsFullscreen)
        {
            m_Swapchain->SetFullscreenState(FALSE, NULL);
            m_IsFullscreen = false;
        }
        m_PresentFlags = 0;

        m_Swapchain->Release();
        m_Swapchain = nullptr;
    }

    bool Swapchain::Present(const WindowState& windowState)
    {
        HRESULT hr = m_Swapchain->Present(windowState.presentInterval, m_PresentFlags);
        return SUCCEEDED(hr);
    }

    ID3D12Resource* Swapchain::GetBuffer(UINT index)
    {
        BLK_ASSERT(m_Swapchain != nullptr);
        BLK_ASSERT(index < BLK_IN_FLIGHT_FRAMES);

        ID3D12Resource* buffer = nullptr;
        HRESULT hr = m_Swapchain->GetBuffer(index, IID_PPV_ARGS(&buffer));

        if (FAILED(hr))
            return nullptr;

        return buffer;
    }

    UINT Swapchain::GetCurrentFrameIndex()
    {
        BLK_ASSERT(m_Swapchain != nullptr);

        return m_Swapchain->GetCurrentBackBufferIndex();
    }

    bool Swapchain::Update(Device& device, WindowState windowState)
    {
        device.Flush();
        HRESULT hr = m_Swapchain->ResizeBuffers(BLK_IN_FLIGHT_FRAMES, windowState.width,
                                                windowState.height, DXGI_FORMAT_B8G8R8A8_UNORM,
                                                DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
        BLK_ASSERT_VAR(SUCCEEDED(hr));
        device.Flush();

        return true;
    }

} // namespace Boolka