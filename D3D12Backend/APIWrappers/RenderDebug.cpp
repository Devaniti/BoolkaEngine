#include "stdafx.h"

#include "RenderDebug.h"

namespace Boolka
{

    bool RenderDebug::Initialize()
    {
#ifdef BLK_RENDER_DEBUG
        {
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
            else
            {
                ::MessageBoxW(NULL, L"Debug Layer Error",
                              L"Can't enable debug layer. Please install \"Graphic Tools\" package",
                              MB_ICONWARNING);
            }
        }
#endif

#ifdef BLK_RENDER_DEVICE_LOST_DEBUG
        {
            ID3D12DeviceRemovedExtendedDataSettings* dredSettings = nullptr;
            HRESULT hr = ::D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings));
            if (SUCCEEDED(hr))
            {
                dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
                dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
            }
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

#ifdef BLK_RENDER_DEBUG
    void RenderDebug::SetDebugName(ID3D12DeviceChild* object, const wchar_t* format, ...)
    {
        va_list argList;
        va_start(argList, format);
        wchar_t name[100];
        // Parses printf format and parameters from va_list and create wide char
        // string
        _vsnwprintf_s(name, ARRAYSIZE(name), format, argList);
        va_end(argList);
        object->SetName(name);
    }
#endif

} // namespace Boolka