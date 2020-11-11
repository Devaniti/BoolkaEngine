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
        return CheckOptions(device) &&
               CheckShaderModel(device);
    }

    void FeatureSupportHelper::Unload()
    {
    }

    bool FeatureSupportHelper::CheckOptions(Device& device)
    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
        HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));
        if (FAILED(hr)) return false;

        return options.ResourceBindingTier >= D3D12_RESOURCE_BINDING_TIER_3;
    }

    bool FeatureSupportHelper::CheckShaderModel(Device& device)
    {
        return true;

        D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
        HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
        if (FAILED(hr)) return false;

        return shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_0;
    }

}