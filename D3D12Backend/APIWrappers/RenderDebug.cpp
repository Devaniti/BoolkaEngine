#include "stdafx.h"

#include "RenderDebug.h"

namespace Boolka
{

    bool RenderDebug::Initialize()
    {
#ifdef BLK_RENDER_DEBUG
        ID3D12Debug3* d3d12DebugInterface = nullptr;
        HRESULT hr = ::D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12DebugInterface));
        if (SUCCEEDED(hr))
        {
            d3d12DebugInterface->EnableDebugLayer();
#ifdef BLK_USE_GPU_VALIDATION
            d3d12DebugInterface->SetEnableGPUBasedValidation(TRUE);
#endif
            d3d12DebugInterface->Release();
        }
#endif
        return true;
    }

    void RenderDebug::Unload()
    {
#ifdef BLK_RENDER_DEBUG
        IDXGIDebug* dxgiDebugInterface = nullptr;
        HRESULT hr = ::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebugInterface));
        if (SUCCEEDED(hr))
        {
            dxgiDebugInterface->ReportLiveObjects(DXGI_DEBUG_DX, DXGI_DEBUG_RLO_ALL);
            dxgiDebugInterface->Release();
        }
#endif
    }

} // namespace Boolka