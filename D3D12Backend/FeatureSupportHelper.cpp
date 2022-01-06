#include "stdafx.h"

#include "FeatureSupportHelper.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    FeatureSupportHelper::FeatureSupportHelper()
    {
    }

    FeatureSupportHelper::~FeatureSupportHelper()
    {
    }

    bool FeatureSupportHelper::Initialize(Device& device)
    {
        D3D12_FEATURE_DATA_SHADER_CACHE shaderCache{};
        HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_CACHE, &shaderCache,
                                                 sizeof(shaderCache));
        if (FAILED(hr) || ((shaderCache.SupportFlags & D3D12_SHADER_CACHE_SUPPORT_LIBRARY) == 0))
        {
            ::MessageBoxW(0, L"ID3D12PipelineLibrary support required",
                          L"GPU and/or driver unsupported", MB_OK | MB_ICONERROR);
            BLK_CRITICAL_DEBUG_BREAK();
            return false;
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS options{};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));
        if (FAILED(hr) || options.ResourceBindingTier < D3D12_RESOURCE_BINDING_TIER_3)
        {
            ::MessageBoxW(0, L"Resource Binding Tier 3 Required", L"GPU and/or driver unsupported",
                          MB_OK | MB_ICONERROR);
            BLK_CRITICAL_DEBUG_BREAK();
            return false;
        }

        D3D12_FEATURE_DATA_SHADER_MODEL shaderModel{D3D_SHADER_MODEL_6_5};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel,
                                         sizeof(shaderModel));
        if (FAILED(hr) || shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5)
        {
            ::MessageBoxW(0, L"Shader Model 6.5 Required", L"GPU and/or driver unsupported",
                          MB_OK | MB_ICONERROR);
            BLK_CRITICAL_DEBUG_BREAK();
            return false;
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5{};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5));
        if (FAILED(hr) || options5.RaytracingTier < D3D12_RAYTRACING_TIER_1_0)
        {
            ::MessageBoxW(0, L"Raytracing Tier 1.0 Required", L"GPU and/or driver unsupported",
                          MB_OK | MB_ICONERROR);
            BLK_CRITICAL_DEBUG_BREAK();
            return false;
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7{};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7));
        if (FAILED(hr) || options7.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
        {
            ::MessageBoxW(0, L"Mesh Shaders Required", L"GPU and/or driver unsupported",
                          MB_OK | MB_ICONERROR);
            BLK_CRITICAL_DEBUG_BREAK();
            return false;
        }

        return true;
    }

    void FeatureSupportHelper::Unload()
    {
    }

} // namespace Boolka